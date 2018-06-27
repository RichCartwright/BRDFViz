#ifndef __RANDOM_UTILS_HPP__
#define __RANDOM_UTILS_HPP__

#include "glm.hpp"

class RandomUtils{
public:
    static float Sample1DToRange(float sample, float begin, float end){
        qassert_true(end >= begin);
        return begin + sample * (end - begin);
    }
    static glm::vec2 Sample2DToDiscUniform(glm::vec2 sample){
        float r = glm::sqrt(sample.x);
        float a = sample.y * 2.0f * M_PI;
        return {r * glm::sin(a), r * glm::cos(a)};
    }
    // Hemisphere of points whose Y > 0
    static glm::vec3 Sample2DToHemisphereUniform(glm::vec2 sample){
        float k1 = sample.x;
        float k2 = sample.y*2.0f - 1.0f;
        float z = 1.0f - k2*k2;
        float s = glm::sqrt(z);
        float w = 2.0f*glm::pi<float>()*k1;
        float x = glm::cos(w) * s;
        float y = glm::sin(w) * s;
        return {x,glm::abs(z),y};
    }
    // Returns a uniformly-distributed random vector in direction of V.
    static glm::vec3 Sample2DToHemisphereUniformDirected(glm::vec2 sample, glm::vec3 direction){
        return MoveToDir(direction, Sample2DToHemisphereUniform(sample));
    }
    // Returns a cosine-distributed random vector on a hemisphere, such that y > 0.
    static glm::vec3 Sample2DToHemisphereCosine(glm::vec2 sample){
        glm::vec2 p = Sample2DToDiscUniform(sample);
        float y = glm::sqrt(glm::max(0.00001f, 1- p.x*p.x - p.y*p.y));
        return glm::vec3(p.x, y, p.y);
    }
    // Returns a cosine-distributed random vector on a hemisphere, such that z > 0.
    static glm::vec3 Sample2DToHemisphereCosineZ(glm::vec2 sample){
        glm::vec2 p = Sample2DToDiscUniform(sample);
        float z = glm::sqrt(glm::max(0.00001f, 1- p.x*p.x - p.y*p.y));
        return glm::vec3(p.x, p.y, z);
    }
    // Returns a cosine-distributed random vector in direction of V.
    static glm::vec3 Sample2DToHemisphereCosineDirected(glm::vec2 sample, glm::vec3 direction){
        return MoveToDir(direction, Sample2DToHemisphereCosine(sample));
    }
    // Returns a uniformly random vector on a sphere
    static glm::vec3 Sample2DToSphereUniform(glm::vec2 sample){
        float z = sample.x * 2.0f - 1.0f;
        float a = sample.y * 6.283185;
        float r = glm::sqrt(1 - z * z);
        float x = r * glm::cos(a);
        float y = r * glm::sin(a);
        return glm::vec3(x, y, z);
    }
    // This function is useful for reusing the same sample multiple
    // times.  It returns whether the sample is within requested
    // probability range, and then scales the sample so that, under
    // the tested condition, it still has range 0-1.
    // For example usage, see BRDFLTCBeckmann::GetRay which gets only
    // two samples, but needs three.
    static bool DecideAndRescale(float& sample, float probability){
        if(probability == 0.0f) return false;
        if(probability == 1.0f) return true;
        if(sample < probability){
            sample /= probability;
            return true;
        }else{
            sample = (sample - probability) / (1.0f - probability);
            return false;
        }
    }
private:
    // TODO: This should be a generic function
    static glm::vec3 MoveToDir(glm::vec3 direction, glm::vec3 r){
        return RotationFromY(direction) * r;
    }
};

#endif //__RANDOM_UTILS_HPP__
