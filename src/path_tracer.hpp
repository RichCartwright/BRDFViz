#ifndef __PATH_TRACER_HPP__
#define __PATH_TRACER_HPP__

#include <cmath>
#include <stddef.h> 
#include <stdlib.h> 
#include <string.h>

//#include "NeuralNet/rtwtypes.h"
//#include "NeuralNet/SibnekLev_types.h"
#include "tracer.hpp"
#include "primitives.hpp"

class Sampler;

class PathTracer : public Tracer{
public:
    PathTracer(const Scene& scene,
               const Camera& camera,
               unsigned int xres,
               unsigned int yres,
               unsigned int multisample,
               unsigned int depth,
               float clamp,
               float russian,
               float bumpmap_scale,
               bool  force_fresnell,
               unsigned int reverse,
               unsigned int samplerSeed,
	       bool log_data,
	       FILE* dataFile);
protected:
    PixelRenderResult RenderPixel(int x, int y, unsigned int & raycount, bool debug = false) override;

private:
    PixelRenderResult TracePath(const Ray& r, unsigned int& raycount, Sampler& sampler, bool debug = false);
    struct PathPoint{
        bool infinity = false;
        // Point position
        glm::vec3 pos;
        // Normal vectors
        glm::vec3 lightN;
        glm::vec3 faceN;
        SystemTransform transform;
        // reflected direction (pointing towards previous path point)
        glm::vec3 Vr;
        // incoming  direction (pointing towards next path point)
        glm::vec3 Vi;
        // Material properties at hitpoint
        const Material* mat;
        glm::vec2 texUV;
        Radiance emission;
        // Thinglass encountered on the way of the ray that generated this point
        ThinglassIsections thinglass_isect;
        // Currection for rusian roulette
        float russian_coefficient;
        // These take into account sampling, BRDF, color. Symmetric in both directions.
        Spectrum transfer_coefficients;
        // Same coefficients as above, but cumulated thorough the
        // path. This indicates how much does the light from this
        // point contribute to the beginning of the path.
        Spectrum contribution = Spectrum(1.0f, 1.0f, 1.0f);
        //
        Radiance light_from_source;
        // True if the ray hit the face from outside (CCW)
        bool backside = false;
    };

    std::vector<PathPoint> GeneratePath(Ray direction, unsigned int& raycount, unsigned int depth__, float russian__, Sampler& sampler, bool debug = false, bool light_ray = false) const;

    Radiance ApplyThinglass(Radiance input, const ThinglassIsections& isections, glm::vec3 ray_direction) const;

    //Radiance sky_radiance;
    float clamp;
    float russian;
    unsigned int depth;
    bool force_fresnell;
    unsigned int reverse;
    mutable unsigned int samplerSeed;
    bool log_data;
    FILE* dataFile;

};

#endif // __PATH_TRACER_HPP__
