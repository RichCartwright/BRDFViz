#include "tracer.hpp"
#include "texture.hpp"
#include "global_config.hpp"
#include "utils.hpp"

void Tracer::Render(const RenderTask& task, EXRTexture* output, std::atomic<int>& pixel_count, std::atomic<unsigned int>& ray_count){
    unsigned int pxdone = 0, raysdone = 0;
    for(unsigned int y = task.yrange_start; y < task.yrange_end; y++){
        for(unsigned int x = task.xrange_start; x < task.xrange_end; x++){
            bool debug = false;
#if ENABLE_DEBUG
            if(debug_trace && x == debug_x && y == debug_y) debug = true;
#endif
            PixelRenderResult px = RenderPixel(x, y, raysdone, debug);

            // Temporarily disabled for light tracing
            // output->AddPixel(x, y, px.main_pixel, multisample);
            output->AddPixel(x, y, px.main_pixel, multisample);

            for(const auto& t : px.side_effects){
                int x2 = std::get<0>(t);
                int y2 = std::get<1>(t);
                Radiance r = std::get<2>(t);
                std::cout << "Setting extra pixel at: " << x2 << " " << y2 << " to " << r  << std::endl;
                output->AddPixel(x2, y2, r, 0);
            }

            pxdone++;
            if(pxdone % 100 == 0){
                pixel_count += 100;
                pxdone = 0;
            }
        }
    }
    pixel_count += pxdone;
    ray_count += raysdone;
}
