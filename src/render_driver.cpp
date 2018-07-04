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
#include "scene.hpp"


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
	const RenderTask& task = tasks[0];
	unsigned int c = seedcount++;

	PathTracer rt(scene, camera,
		      task.xres, task.yres,
		      cfg->multisample,
		      cfg->recursion_level,
		      cfg->clamp,
		      cfg->russian,
		      cfg->bumpmap_scale,
		      cfg->force_fresnell,
		      cfg->reverse,
		      seedstart + c);
	
	EXRTexture output_buffer(cfg->xres, cfg->yres);
	rt.Render(task, &output_buffer, pixels_done, rays_done);
	{
	    total_ob.Accumulate(output_buffer);
	}
}

void RenderDriver::RenderFrame()
{
    // Preapare output buffer
    EXRTexture total_ob(cfg->xres, cfg->yres);
    total_ob.Write(output_file);
    out::cout(2) << "Writing to file " << output_file << std::endl;
    // Split rendering into smaller (tile_size x tile_size) tasks.
    glm::vec2 midpoint(cfg->xres/2.0f, cfg->yres/2.0f);
    
    std::vector<RenderTask> tasks = GenerateTaskList(512, cfg->xres, cfg->yres, midpoint);
    std::cout << "Rendering in " << tasks.size() << " tiles." << std::endl;

    unsigned int seedcount = 0, seedstart = 42;
    RenderRound(scene, cfg, camera, tasks, seedcount, seedstart, 1, total_ob);
    total_ob.Normalize(cfg->output_scale).Write(output_file);
}
