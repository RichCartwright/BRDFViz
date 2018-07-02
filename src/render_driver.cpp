#include "render_driver.hpp"

#include <iomanip>

#include <cmath>
#include <thread>
#include <chrono>
#include <algorithm>
#include <unistd.h>

#include "../external/ctpl_stl.h"

#include "path_tracer.hpp"
#include "utils.hpp"
#include "out.hpp"
#include "texture.hpp"
std::chrono::high_resolution_clock::time_point RenderDriver::frame_render_start;
std::atomic<bool> RenderDriver::stop_monitor(false);

// Performance counters
std::atomic<int> RenderDriver::rounds_done(0);
std::atomic<int> RenderDriver::pixels_done(0);
std::atomic<unsigned int> RenderDriver::rays_done(0);
void RenderDriver::ResetCounters(){
    rounds_done = 0;
    pixels_done = 0;
    rays_done = 0;
}

std::vector<RenderTask> GenerateTaskList(unsigned int tile_size,
                                         unsigned int xres,
                                         unsigned int yres,
                                         glm::vec2 middle){
    std::vector<RenderTask> tasks;
    for(unsigned int yp = 0; yp < yres; yp += tile_size){
        for(unsigned int xp = 0; xp < xres; xp += tile_size){
            RenderTask task(xres, yres, xp, std::min(xres, xp+tile_size),
                                        yp, std::min(yres, yp+tile_size));
            tasks.push_back(task);
        }
    }
    std::sort(tasks.begin(), tasks.end(), [&middle](const RenderTask& a, const RenderTask& b){
            return glm::length(middle - a.midpoint) < glm::length(middle - b.midpoint);
        });
    return tasks;
}


void RenderDriver::FrameMonitorThread(RenderLimitMode render_limit_mode,
                                      unsigned int limit_rounds,
                                      unsigned int limit_minutes,
                                      unsigned int pixels_per_round){

    stop_monitor = false;

    // Helper function that prints out the progress bar.
    auto print_progress_f = [=](bool final_print = false){
        static Utils::LowPass eta_lp(40);

        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        float elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - frame_render_start).count() / 1000.0f;
        float elapsed_minutes = elapsed_seconds / 60.0f;

        // Prepare variables and text that varies with render limit mode
        float fraction = 0.0f, eta_seconds = 0.0f;
        std::string pixels_text, rounds_text;
        std::stringstream ss;
        bool mask_eta = false;
        switch(render_limit_mode){
        case RenderLimitMode::Rounds:
            const static unsigned int total_pixels = pixels_per_round * limit_rounds;
            fraction = pixels_done/(float)total_pixels;
            eta_seconds = (1.0f - fraction)*elapsed_seconds/fraction;
            // Smooth ETA with a simple low-pass filter
            eta_seconds = eta_lp.Add(eta_seconds);
            ss << "Rendered " << std::setw(log10(total_pixels) + 1) << pixels_done << "/" << total_pixels << " pixels";
            pixels_text = ss.str();
            ss.str(std::string()); ss.clear();
            ss << "round " << std::min((unsigned int)rounds_done + 1, limit_rounds) << "/" << limit_rounds;
            rounds_text = ss.str();
            mask_eta = (fraction < 0.03f && elapsed_seconds < 20.0f);
            break;
        case RenderLimitMode::Timed:
            fraction = std::min(1.0f, elapsed_minutes/limit_minutes);
            eta_seconds = std::max(0.0f, limit_minutes*60 - elapsed_seconds);
            pixels_text = "Rendered " + std::to_string(pixels_done) + " pixels";
            ss << "round " << rounds_done + (final_print?0:1);
            rounds_text = ss.str();
            mask_eta = false;
            break;
        }

        float percent = int(fraction*1000.0f + 0.5f) / 10.0f;
        // If this is the last time we're displayin the bar, make sure to nicely round numbers
        if(final_print){
            eta_seconds = 0.0f;
            if(render_limit_mode == RenderLimitMode::Rounds){
                fraction = 1.0f;
                percent = 100.0f;
            }
        }
        std::string eta_str = "ETA: " + std::string(((mask_eta)?"???":Utils::FormatTime(eta_seconds)));
        std::string percent_str = Utils::FormatPercent(percent);
        std::string elapsed_str = "time elapsed: " + Utils::FormatTime(elapsed_seconds);
        unsigned int bar_fill = fraction * BARSIZE;
        unsigned int bar_empty = BARSIZE - bar_fill;
        // Output text
        out::cout(1) << "\033[1A"; // Cursor up 1 line
        // Line 1
        out::cout(1) << "\33[2K\r" << "[";
        for(unsigned int i = 0; i < bar_fill ; i++) out::cout(1) << "#";
        for(unsigned int i = 0; i < bar_empty; i++) out::cout(1) << "-";
        out::cout(1) << "] " << percent_str << std::endl;
        // Line 2
        out::cout(1) << "\33[2K\r" << pixels_text << ", " << rounds_text << ", " << elapsed_str << ", " << eta_str;
        out::cout(1).flush();
    };

    out::cout(1) << std::endl;
    while(!stop_monitor){
        print_progress_f();
        //if(pixels_done >= total_pixels) break;
        usleep(1000*100); // 50ms
    }

    // Output the message one more time to display "100%"
    print_progress_f(true);
    std::cout << std::endl;

    // Measure total wallclock time
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    float total_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - frame_render_start).count() / 1000.0f;

    out::cout(2) << "Total frame rendering time: " << Utils::FormatTime(total_seconds) << std::endl;
    out::cout(3) << "Total rays: " << rays_done << std::endl;
    out::cout(2) << "Average pixels per second: " << Utils::FormatIntThousands(pixels_done / total_seconds) << "." << std::endl;
    out::cout(3) << "Average rays per second: " << Utils::FormatIntThousands(rays_done / total_seconds) << std::endl;

}

// TODO: Seed generator should be a standalone object
// TODO: Create a different 'lite config' struct, which will only
// contain render parameters, ideal for passing here
void RenderDriver::RenderRound(const Scene& scene,
                               std::shared_ptr<Config> cfg,
                               const Camera& camera,
                               const std::vector<RenderTask>& tasks,
                               unsigned int& seedcount,
                               const int seedstart,
                               unsigned int concurrency,
                               EXRTexture& total_ob
                               )
{

    // Since RGK isn't the only thing running, 
    //  we only really want to give this 1 thread, 
    //  this can be tweaked later maybe half threads would be better?
    ctpl::thread_pool tpool(1);
    std::mutex total_ob_mx;
    // Push all render tasks to thread pool
    for(unsigned int i = 0; i < tasks.size(); i++)
    {
        const RenderTask& task = tasks[i];
        unsigned int c = seedcount++;
        tpool.push( [seedstart, camera, &scene, &cfg, task, c, &total_ob_mx, &total_ob](int){
                // THIS is the thread task
                PathTracer rt(scene, camera,
                              task.xres, task.yres,
                              cfg->multisample,
                              cfg->recursion_level,
                              cfg->clamp,
                              cfg->russian,
                              cfg->bumpmap_scale,
                              cfg->force_fresnell,
                              cfg->reverse,
                              seedstart + c,
            			      cfg->collect_data,
			                  cfg->dataFile);
                out::cout(6) << "Starting a new task with params: " << std::endl;
                out::cout(6) << "camerapos = " << camera.origin << ", multisample = " << cfg->multisample << ", reclvl = " << cfg->recursion_level << ", russian = " << cfg->russian << ", reverse = " << cfg->reverse << std::endl;
                EXRTexture output_buffer(cfg->xres, cfg->yres);
                rt.Render(task, &output_buffer, pixels_done, rays_done);
                {
                    std::lock_guard<std::mutex> lk(total_ob_mx);
                    total_ob.Accumulate(output_buffer);
                }
         });
    }

    // Wait for all remaining worker threads to complete.
    tpool.stop(true);

    rounds_done++;
}

void RenderDriver::RenderFrame(const Scene& scene,
                               std::shared_ptr<Config> cfg,
                               const Camera& camera,
                               std::string output_file
                               ){
    ResetCounters();

    // Preapare output buffer
    EXRTexture total_ob(cfg->xres, cfg->yres);
    total_ob.Write(output_file);
    out::cout(2) << "Writing to file " << output_file << std::endl;

    // Determine thread pool size.
    unsigned int concurrency = std::thread::hardware_concurrency();
    concurrency = std::max((unsigned int)1, concurrency); // If available, leave one core free. 
    
    concurrency = 1; // Leave some threads for VTK

    out::cout(2) << "Using thread pool of size " << concurrency << std::endl;

    // Split rendering into smaller (tile_size x tile_size) tasks.
    glm::vec2 midpoint(cfg->xres/2.0f, cfg->yres/2.0f);
#if ENABLE_DEBUG
    // However, if debug is enabled, sort tiles so that the debugged point gets rendered earliest.
    if(debug_trace) midpoint = glm::vec2(debug_x, debug_y);
#endif
    std::vector<RenderTask> tasks = GenerateTaskList(512, cfg->xres, cfg->yres, midpoint);
    std::cout << "Rendering in " << tasks.size() << " tiles." << std::endl;
    
    // Start monitor thread.
    std::thread monitor_thread(FrameMonitorThread, cfg->render_limit_mode, cfg->render_rounds, cfg->render_minutes,
                               cfg->xres * cfg->yres);

    unsigned int seedcount = 0, seedstart = 42;

    // Measuring render time, both for timed mode, and monitor output
    frame_render_start = std::chrono::high_resolution_clock::now();

    switch(cfg->render_limit_mode){
    case RenderLimitMode::Rounds:
        for(unsigned int roundno = 0; roundno < cfg->render_rounds; roundno++){
            // Render a single round
            RenderRound(scene, cfg, camera, tasks, seedcount, seedstart, concurrency, total_ob);
            // Write out current progress to the output file.
            total_ob.Normalize(cfg->output_scale).Write(output_file);
        }
        break;
    case RenderLimitMode::Timed:
        while(true){
            // Break loop if too much time elapsed
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            float minutes_elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - frame_render_start).count() / 60.0;
            if(minutes_elapsed >= cfg->render_minutes) break;
            // Render a single round
            RenderRound(scene, cfg, camera, tasks, seedcount, seedstart, concurrency, total_ob);
            // Write out current progress to the output file.
            total_ob.Normalize(cfg->output_scale).Write(output_file);
        }
        break;
    }

    // Shutdown monitor thread.
    stop_monitor = true;
    if(monitor_thread.joinable()) monitor_thread.join();
}
