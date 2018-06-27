#ifndef __RAY_HPP__
#define __RAY_HPP__

#include "glm.hpp"

class Ray{
public:
    Ray() {}
    // Directional constructor (half-line).
    Ray(glm::vec3 from, glm::vec3 dir) :
        origin(from){
        direction = glm::normalize(dir);
    }
    // From-to constructor. Sets near and far.
    Ray(glm::vec3 from, glm::vec3 to, float epsillon){
        origin = from;
        glm::vec3 diff = to-from;
        direction = glm::normalize(diff);
        float length = glm::length(diff);
        near =   0.0f + epsillon;
        far  = length - epsillon;
    }
    glm::vec3 origin;
    glm::vec3 direction;
    float near = 0.0f;
    float far = 10000.0f;
    glm::vec3 t(float t) const {return origin + t*direction;}
    inline glm::vec3 operator[](float t_) const {return t(t_);}
};

#endif // __RAY_HPP__
