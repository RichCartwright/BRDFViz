#include "path_tracer.hpp"

#include "camera.hpp"
#include "scene.hpp"
#include "global_config.hpp"
#include "random_utils.hpp"
#include "sampler.hpp"
#include "bxdf/bxdf.hpp"
#include "utils.hpp"
//#include "NeuralNet/rt_nonfinite.h"
//#include "NeuralNet/SibnekLev.h"
//#include "NeuralNet/SibnekLev_terminate.h" 
//#include "NeuralNet/SibnekLev_initialize.h"

#include <tuple>
#include <iostream>

#include "glm.hpp"
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>
/*
//For the neural net
static double dv4[18]; //input 
static double b_y1[3]; //output
static void argInt_1x15_real_T(double result[18]); 
static double argInit_real_T();
static void main_SibnekLev();
static int bufferCounter = 0; 
static void argInt_1x15_real_T(double result[18])
{
	int idx1;

	for (idx1 = 0; idx1 < 18; idx1++)
	{
		result[idx1] = argInit_real_T();
	}
}

static double argInit_real_T()
{
	return 0.0;
}

static void main_SibnekLev()
{
	//dv4 = input & b_y1 = output
	SibnekLev(dv4, b_y1);
}
*/
PathTracer::PathTracer(const Scene& scene,
                       const Camera& camera,
                       unsigned int xres,
                       unsigned int yres,
                       unsigned int multisample,
                       unsigned int depth,
                       float clamp,
                       float russian,
                       float bumpmap_scale,
                       bool force_fresnell,
                       unsigned int reverse,
                       unsigned int samplerSeed)
: Tracer(scene, camera, xres, yres, multisample, bumpmap_scale),
  clamp(clamp),
  russian(russian),
  depth(depth),
  force_fresnell(force_fresnell),
  reverse(reverse),
  samplerSeed(samplerSeed)
{
}

PixelRenderResult PathTracer::RenderPixel(int x, int y, unsigned int & raycount, bool debug){
    PixelRenderResult total;

    IFDEBUG std::cout << std::endl;

    samplerSeed += 0x42424242;
    //LatinHypercubeSampler sampler(samplerSeed, 64, multisample);
//    StratifiedSampler sampler(samplerSeed, 64, multisample);
    //VanDerCoruptSampler sampler(samplerSeed, 64, multisample);
    //HaltonSampler sampler(samplerSeed, 64, multisample);
    IndependentSampler sampler(samplerSeed);
    //HoradamSampler sampler(samplerSeed);
    for(unsigned int i = 0; i < multisample; i++)
    {
        sampler.Advance();
        IFDEBUG std::cout << "[SAMPLER] Advaincing sampler" << std::endl;
	
	    glm::vec3 outpos;
        glm::vec2 coords = sampler.Get2D();
        Ray r = camera.IsSimple() ?
            camera.GetPixelRay(x, y, xres, yres, coords, &outpos) :
            camera.GetPixelRayLens(x, y, xres, yres, coords, sampler.Get2D());

        // First, lets enter the current pixel number
        double pixel[2] = { (double)x, (double)y };
        pathData.insert(pathData.end(), std::begin(pixel), std::end(pixel));

        // for the sake of ease, just load the cam positions and colours into an array
        double p[6] = {outpos.x, outpos.y, outpos.z, 0.0, 1.0, 0.0}; 
	    pathData.insert(pathData.end(), std::begin(p), std::end(p));
        
        PixelRenderResult q = TracePath(r, raycount, sampler, debug);
        total.main_pixel += q.main_pixel;

        for(const auto& p : q.side_effects)
        {
            total.side_effects.push_back(p);
        }

        IFDEBUG std::cout << "Side effects: " << q.side_effects.size() << std::endl;
        IFDEBUG std::cout << "[SAMPLER] Samples used for this ray: " << sampler.GetUsage().first + sampler.GetUsage().second << std::endl;
    }

    IFDEBUG std::cout << "-----> pixel average: " << total.main_pixel/multisample << std::endl << std::endl;

    return total;
}


Radiance PathTracer::ApplyThinglass(Radiance input, const ThinglassIsections& isections, glm::vec3 ray_direction) const {
    Radiance result = input;
    float ct = -1.0f;
    for(int n = isections.size()-1; n >= 0; n--){
        const Triangle* trig = std::get<0>(isections[n]);
        // Ignore repeated triangles within epsillon radius from
        // previous thinglass - they are probably clones of the same
        // triangle in kd-tree.
        float newt = std::get<1>(isections[n]);
        if(newt <= ct + scene.epsilon) continue;
        ct = newt;
        // This is just to check triangle orientation, so that we only
        // apply color filter when the ray is entering glass.
        glm::vec3 N = trig->generic_normal();
        if(glm::dot(N,ray_direction) >= 0){
            //glm::vec2 UV = std::get<2>(isections[n]);
            // TODO: Use translucency filter instead of diffuse!
            // TODO: Respect texture UV coordinates

            // TODO: This is obsolete, use better BxDF instead
            /*
            Color c = trig->GetMaterial().diffuse->Get(glm::vec2(0.0, 0.0));
            result = result * Spectrum(c);
            */
        }
    }
    return result;
}

std::vector<PathTracer::PathPoint> PathTracer::GeneratePath(Ray r, unsigned int& raycount, unsigned int depth__, float russian__, Sampler& sampler, bool debug, bool light_ray) const {

    std::vector<PathPoint> path;

    //if (!light_ray) std::cout << "Ray origin: " << r.origin << std::endl;
   // std::cout << "Ray direction: " << r.direction << std::endl;

    Spectrum cumulative_transfer_coefficients = Spectrum(1.0f, 1.0f, 1.0f);

    Ray current_ray = r;
    unsigned int n = 0;
    const Triangle* last_triangle = nullptr;
    while(n < depth__){
        n++;
        IFDEBUG std::cout << "Generating path, n = " << n << std::endl;

        raycount++;
        Intersection i;
        if(scene.thinglass.size() == 0){
            // This variant is a bit faster.
            i = scene.FindIntersectKdOtherThan(current_ray, last_triangle);
        }else{
            i = scene.FindIntersectKdOtherThanWithThinglass(current_ray, last_triangle);
        }
        PathPoint p;
        p.contribution = cumulative_transfer_coefficients;
        p.thinglass_isect = i.thinglass;

        //std::cout << "Point " << n << ":" << current_ray[i.t] << std::endl;
	
        if(!i.triangle){
           // A sky ray!
            IFDEBUG std::cout << "Sky ray!" << std::endl;
            p.infinity = true;
            p.Vr = -current_ray.direction;
            qassert_false(std::isnan(p.Vr.x));
            path.push_back(p);
            // End path.
            break;
        }else{
            if(i.triangle == last_triangle){
                // std::cerr << "Ray collided with source triangle. This should never happen." << std::endl;
            }
            // Prepare normal
            assert(NEAR(glm::length(current_ray.direction),1.0f));
            p.pos = current_ray[i.t];

            p.faceN = i.Interpolate(i.triangle->GetNormalA(),
                                    i.triangle->GetNormalB(),
                                    i.triangle->GetNormalC());

            if(std::isnan(p.faceN.x)){
                // Ah crap. Assimp incorrectly merged some vertices.
                // Just try another normal.
                p.faceN = i.triangle->GetNormalA();
                if(std::isnan(p.faceN.x)){
                    p.faceN = i.triangle->GetNormalB();
                    if(std::isnan(p.faceN.x)){
                        p.faceN = i.triangle->GetNormalC();
                        if(std::isnan(p.faceN.x)){
                            // All three vertices are messed up? Not much we can help now. Let's just ignore this ray.
				return path;
                        }
                    }
                }
            }
            // Sometimes it may happen, when interpolating between reverse vectors,
            // the the the result is 0. Or worse: some models contain zero-length normal vectors!
            // In such unfortunate case, just igore this ray.
            if(glm::length(p.faceN) <= 0.0f){
                return path;
            }

            p.faceN = glm::normalize(p.faceN);
            // Prepare incoming direction
            p.Vr = -current_ray.direction;
            qassert_false(std::isnan(p.Vr.x));

            // Invert normal in case this ray would enter from inside
            // if(glm::dot(p.faceN, p.Vr) <= 0.0f) p.faceN = -p.faceN;

            const Material& mat = i.triangle->GetMaterial();
            p.mat = &mat;

            assert(!std::isnan(p.faceN.x));

            // Interpolate textures
            glm::vec2 a = i.triangle->GetTexCoordsA();
            glm::vec2 b = i.triangle->GetTexCoordsB();
            glm::vec2 c = i.triangle->GetTexCoordsC();
            p.texUV = i.Interpolate(a,b,c);
            IFDEBUG std::cout << "texUV = " << p.texUV << std::endl;

            // Get colors from texture
            // TODO: Single-color values should also be processed as textures
            p.emission = mat.emission;

            // Tilt normal using bump texture
            if(!mat.bumpmap->Empty()){
                float right = mat.bumpmap->GetSlopeRight(p.texUV);
                float bottom = mat.bumpmap->GetSlopeBottom(p.texUV);
                glm::vec3 tangent = i.Interpolate(i.triangle->GetTangentA(),
                                                  i.triangle->GetTangentB(),
                                                  i.triangle->GetTangentC());
                if(tangent.x*tangent.x + tangent.y*tangent.y + tangent.z*tangent.z < 0.001f){
                    // Well, so apparently, sometimes assimp generates invalid tangents. They seem okay
                    // on their own, but they interpolate weird, because tangents at two coincident vertices
                    // are opposite. Thus if it happens that interpolated tangent is zero, and therefore can't be
                    // normalized, we just silently ignore the bump map in this point. I'll have little effect on the
                    // entire pixel anyway.
                    p.lightN = p.faceN;
                }else{
                    tangent = glm::normalize(tangent);
                    glm::vec3 bitangent = glm::normalize(glm::cross(p.faceN,tangent));
                    glm::vec3 tangent2 = glm::cross(bitangent,p.faceN);
                    p.lightN = glm::normalize(p.faceN + (tangent2*right + bitangent*bottom) * bumpmap_scale);
                    IFDEBUG std::cout << "faceN " << p.faceN << std::endl;
                    IFDEBUG std::cout << "lightN " << p.lightN << std::endl;
                    // This still happend.
                    if(glm::isnan(p.lightN.x)){
                        p.lightN = p.faceN;
                    }
                }
            }else{
                p.lightN = p.faceN;
            }

            assert(!std::isnan(p.lightN.x));

            p.transform = SystemTransform(p.lightN, BxDFUpVector);

            // Compute next ray direction
            IFDEBUG std::cout << "Ray hit material " << mat.name << " at " << p.pos << std::endl;
            glm::vec3 dir;
            glm::vec2 sample;

            bool may_leak;
            sample = sampler.Get2D();
            std::tie(dir, p.transfer_coefficients, may_leak) =
                mat.bxdf->sample(p.transform.toLocal(p.Vr),
                                 p.texUV,
                                 sample,
                                 debug);
            bool inside = dir.z < 0;
            dir = p.transform.toGlobal(dir);
#define sameSign(x,y) (x*y > 0)
            if(!sameSign(glm::dot(dir, p.faceN), glm::dot(p.Vr, p.faceN)) && !may_leak){
                // Huh. The next bump is right here on this very same face.
                // TODO: Do not add epsilon when creating next ray!
                // TODO: Maybe the path should terminate here?
                IFDEBUG std::cout << "Ray leaked when it should not, terminating path after this point" << std::endl;
                //IFDEBUG std::cout << glm::dot(dir, p.faceN) << std::endl;
                //IFDEBUG std::cout << glm::dot(p.Vr, p.faceN) << std::endl;
                //n += 10000;
            }

            p.Vi = dir;

            // Store russian coefficient
            if(!mat.no_russian && russian__ > 0.0f && n > 1) p.russian_coefficient = 1.0f/russian__;
            else p.russian_coefficient = 1.0f;

            cumulative_transfer_coefficients *= p.russian_coefficient;
            cumulative_transfer_coefficients *= p.transfer_coefficients;
            IFDEBUG std::cout << "Path cumulative transfer coeff: " << cumulative_transfer_coefficients << std::endl;

            // Commit the path point to the path
            path.push_back(p);

	    /*
            if(cumulative_transfer_coefficients.max() < 0.001f){
                // Terminate when cumulative_coeff gets too low
                IFDEBUG std::cout << "Terminating, cumulative transfer too small." << std::endl;
                break;
            }
*/
	    /*
            // Russian roulette path termination
            if(!mat.no_russian && russian__ >= 0.0f && sampler.Get1D() > russian__){
                IFDEBUG std::cout << "Russian terminating." << std::endl;
                break;
            }
*/
            // Fixed depth path termination
            if(n > depth__) break;

            // Prepate next ray
            current_ray = Ray(p.pos +
                              p.faceN * scene.epsilon * 10.0f *
                              // TODO: Correction depending on outcoming vector side
                              (inside?-1.0f:1.0f)
                              , glm::normalize(dir));
            qassert_false(std::isnan(current_ray.direction.x));

            IFDEBUG std::cout << "Next ray will be from " << p. pos << " dir " << dir << std::endl;

            last_triangle = i.triangle;
            // Continue for next ray
        }
    }

    return path;
}

PixelRenderResult PathTracer::TracePath(const Ray& r, unsigned int& raycount, Sampler& sampler, bool debug){
    PixelRenderResult result;
    glm::vec3 camerapos = r.origin;
    // Choose a main light source.
    glm::vec2 areal_sample = sampler.Get2D();

    IFDEBUG std::cout << "[SAMPLER] Areal sample:" << areal_sample << " from 2D sample #" << sampler.GetUsage().second-1 << std::endl;

    glm::vec2 lightdir_sample = sampler.Get2D();

    std::vector<Light> lights;
    lights.push_back(scene.GetRandomLight(sampler.Get2D(), sampler.Get1D(), areal_sample, debug));

    // ===== 1st Phase =======
    // Generate a forward path.
    IFDEBUG std::cout << "== FORWARD PATH" << std::endl;
    std::vector<PathPoint> path = GeneratePath(r, raycount, depth, russian, sampler, debug);

    // Choose auxiculary light sources
    /*
    const int AUX_LIGHTS = 4;
    for(unsigned int i = 0; i < AUX_LIGHTS; i++)
        lights.push_back(scene.GetRandomLight(rnd));
    */

    // Generate backward path (from light)
    Light& main_light = lights[0];
    glm::vec3 main_light_dir;
    if(main_light.type == Light::FULL_SPHERE){
        glm::vec3 dir = RandomUtils::Sample2DToSphereUniform(areal_sample);
        // TODO: Can this be done without modifying light position?
        main_light.pos += main_light.size * dir;
        main_light_dir = RandomUtils::Sample2DToHemisphereCosineDirected(lightdir_sample, glm::normalize(dir));
    }else{
        main_light_dir = RandomUtils::Sample2DToHemisphereCosineDirected(lightdir_sample, main_light.normal);
    }
    IFDEBUG std::cout << "== LIGHT PATH" << std::endl;
    Ray light_ray(main_light.pos + scene.epsilon * main_light.normal * 100.0f, main_light_dir);
    std::vector<PathPoint> light_path = GeneratePath(light_ray, raycount, reverse, -1.0f, sampler, debug, true);
    IFDEBUG std::cout << "Light path size " << light_path.size() << std::endl;

    // ============== 2nd phase ==============
    // Calculate light transmitted over light path.

    //glm::vec3 Vi = glm::normalize(lightpos - p.pos);
    //float G = glm::max(0.0f, glm::dot(p.lightN, Vi)) / glm::distance2(lightpos, p.pos);
    //IFDEBUG std::cout << "G = " << G << std::endl;
    IFDEBUG std::cout << "main_light.pos = " << main_light.pos << std::endl;
    Radiance light_at_path_start =
        Radiance(main_light.color) *
        Spectrum (main_light.intensity *
                  main_light.GetDirectionalFactor(main_light_dir)
                  ); // * G;;

    IFDEBUG std::cout << " === Carrying light along light path" << std::endl;

    for(unsigned int n = 0; n < light_path.size(); n++){
        PathPoint& p = light_path[n];

        Radiance light_here = p.contribution * light_at_path_start;

        p.light_from_source = light_here;

        IFDEBUG std::cout << "At point " << n << ", light from path start reachin this point: " << light_here << std::endl;

        // Connect the point with camera and add as a side effect
        if(!p.infinity && scene.Visibility(p.pos, camerapos)){
            IFDEBUG std::cout << "Point " << p.pos << " is visible from camera." << std::endl;
            glm::vec3 direction = glm::normalize(p.pos - camerapos);
            Radiance q = light_here *
                p.mat->bxdf->value(p.transform.toLocal(p.Vr),
                                   p.transform.toLocal(-direction),
                                   p.texUV,
                                   debug);
            float G = glm::max(0.0f, glm::dot(p.lightN, -direction)) / glm::distance2(camerapos, p.pos);
            IFDEBUG std::cout << "G = " << G << std::endl;
            if(G >= 0.00001f && !std::isnan(q.r)){
                q *= Spectrum(G);
                int x2, y2;
                IFDEBUG std::cout << "Side effect from " << direction << std::endl;
                bool in_view = camera.GetCoordsFromDirection( direction, x2, y2, debug);
                if(in_view){
                    IFDEBUG std::cout << "In view at " << x2 << " " << y2 << ", radiance: " << q << std::endl;
                    result.side_effects.push_back(std::make_tuple(x2, y2, q));
                }
            }
        }
    }

    // ============== 3rd phase ==============
    // Calculate light transmitted over view path.

    Radiance path_total = Radiance(0.0f, 0.0f, 0.0f);

    for(unsigned int n = 0; n < path.size(); n++)
    {
        IFDEBUG std::cout << "--- Processing PP " << n << std::endl;

        const PathPoint& p = path[n];

        pathData.insert(pathData.end(), {p.pos.x, p.pos.y, p.pos.z});
	    if(p.infinity)
        {
            qassert_false(std::isnan(p.Vr.x));
            Radiance sky_radiance = scene.GetSkyboxRay(p.Vr, debug);
            IFDEBUG std::cout << "This a sky ray, total: " << sky_radiance << std::endl;
            IFDEBUG std::cout << "contribution: " << p.contribution << std::endl;
            path_total += p.contribution * ApplyThinglass(sky_radiance, p.thinglass_isect, -p.Vr);
	        continue;
        }
        const Material& mat = *p.mat;

        IFDEBUG std::cout << "Hit material: " << mat.name << std::endl;

        Radiance total_here(0.0,0.0,0.0);

        // ==========
        // Direct lighting

        const Light& light = main_light;

        ThinglassIsections thinglass_isect;
        // Visibility factor
        if((scene.thinglass.size() == 0 && scene.Visibility(light.pos, p.pos)) ||
           (scene.thinglass.size() != 0 && scene.VisibilityWithThinglass(light.pos, p.pos, thinglass_isect))){

            IFDEBUG std::cout << "====> Light is visible" << std::endl;

            // Incoming direction
            glm::vec3 Vi = glm::normalize(light.pos - p.pos);

            Spectrum f = mat.bxdf->value(p.transform.toLocal(Vi),
                                         p.transform.toLocal(p.Vr),
                                         p.texUV,
                                         debug);

            IFDEBUG std::cout << "f = " << f << std::endl;

            float G = glm::abs(glm::dot(p.lightN, Vi)) / glm::distance2(light.pos, p.pos);
            IFDEBUG std::cout << "G = " << G << ", angle " << glm::angle(p.lightN, Vi) << std::endl;
            Radiance inc_l = Radiance(light.color) * Spectrum( light.intensity *
                                                               light.GetDirectionalFactor(-Vi)
                                                               );
            inc_l = ApplyThinglass(inc_l, thinglass_isect, Vi);

            IFDEBUG std::cout << "incoming light with filters: " << inc_l << std::endl;

            Radiance out = inc_l * ( f * G );
            IFDEBUG std::cout << "total direct lighting: " << out << std::endl;
            total_here += out;
        }else{
            IFDEBUG std::cout << "Light not visible" << std::endl;
        }

        // Reverse light
        for(unsigned int q = 0; q < light_path.size(); q++){
            const PathPoint& l = light_path[q];
            // TODO: Thinglass?
            if(!l.infinity && scene.Visibility(l.pos, p.pos)){
                glm::vec3 light_to_p = glm::normalize(p.pos - l.pos);
                glm::vec3 p_to_light = -light_to_p;
                Spectrum f_light = l.mat->bxdf->value(l.transform.toLocal(light_to_p),
                                                      l.transform.toLocal(l.Vr),
                                                      l.texUV,
                                                      debug);
                Spectrum f_point = p.mat->bxdf->value(p.transform.toLocal(p.Vr),
                                                          p.transform.toLocal(p_to_light),
                                                      p.texUV,
                                                      debug);
                float G = glm::abs(glm::dot(p.lightN, p_to_light)) / glm::distance2(l.pos, p.pos);
                total_here += l.light_from_source * ( f_light * f_point * G );
            }// not visible from each other.
        }

        IFDEBUG std::cout << "total with light path: " << total_here << std::endl;


        if(glm::dot(p.faceN, p.Vr) > 0){
            total_here += p.emission; /* * glm::dot(p.lightN, p.Vr); */
        }


        IFDEBUG std::cout << "total here: " << total_here << std::endl;
        IFDEBUG std::cout << "contribution: " << p.contribution << std::endl;

        pathData.insert(pathData.end(), {total_here.r, total_here.g, total_here.b});
        total_here.clamp(clamp);
        IFDEBUG std::cout << "total here clamped: " << total_here << std::endl;

        path_total += total_here * p.contribution;

    } // for each point on path
   
    // NEURAL NET STUFF
    //main_SibnekLev(); //Updates b_y1 with the NN result. Uses dy_4 as an input
    //Color NeuralResult = Color(b_y1[0], b_y1[1], b_y1[2]);
    //bufferCounter = 0; //Reset the array itterator 
   // path_total = Radiance(NeuralResult);

    path_total.clamp(clamp); //Relative to the JSON, if no input its 100000.f for some reason 

    // Lets get the path value before its gets messed with
    // Safeguard against any spontenous nans or negative values.
    if(glm::isnan(path_total.r) || path_total.r < 0.0f) path_total.r = 0.0f;
    if(glm::isnan(path_total.g) || path_total.g < 0.0f) path_total.g = 0.0f;
    if(glm::isnan(path_total.b) || path_total.b < 0.0f) path_total.b = 0.0f;

    pathData.insert(pathData.end(), {path_total.r*255, path_total.g*255, path_total.b*255}); 
    emit ReturnPathData(pathData);
    pathData.clear(); 
    //SibnekLev_terminate(); //Clean up the NN 
   
    IFDEBUG std::cout << "PATH TOTAL" << path_total << std::endl << std::endl;
    result.main_pixel = path_total;
    return result;

}
