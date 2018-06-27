#ifndef __TRACER_HPP__
#define __TRACER_HPP__

#include <vector>
#include <atomic>
#include <tuple>

#include "glm.hpp"
#include "radiance.hpp"
class Scene;
class Camera;
class EXRTexture;

struct RenderTask{
    RenderTask(unsigned int xres, unsigned int yres, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2)
        : xres(xres), yres(yres), xrange_start(x1), xrange_end(x2), yrange_start(y1), yrange_end(y2)
    {
        midpoint = glm::vec2((xrange_start + xrange_end)/2.0f, (yrange_start + yrange_end)/2.0f);
    }
    unsigned int xres, yres;
    unsigned int xrange_start, xrange_end;
    unsigned int yrange_start, yrange_end;
    glm::vec2 midpoint;
};

struct PixelRenderResult{
    PixelRenderResult() {};
    PixelRenderResult(Radiance main) : main_pixel(main) {}
    Radiance main_pixel;
    std::vector<std::tuple<int,int,Radiance>> side_effects;
};

class Tracer{
protected:
    Tracer(const Scene& scene,
           const Camera& camera,
           unsigned int xres,
           unsigned int yres,
           unsigned int multisample,
           float bumpmap_scale = 10.0)
        : scene(scene),
          camera(camera),
          xres(xres), yres(yres),
          multisample(multisample),
          bumpmap_scale(bumpmap_scale)
    {}

public:
    void Render(const RenderTask& task, EXRTexture* output, std::atomic<int>& pixel_count, std::atomic<unsigned int>& ray_count);

protected:
    virtual PixelRenderResult RenderPixel(int x, int y, unsigned int & raycount, bool debug = false) = 0;

    const Scene& scene;
    const Camera& camera;
    unsigned int xres, yres;
    unsigned int multisample;

    float bumpmap_scale;
};

#endif // __TRACER_HPP__
