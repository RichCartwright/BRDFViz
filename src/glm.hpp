#ifndef __GLM_HPP__
#define __GLM_HPP__

#include "global_config.hpp"

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

// Same as above, optimized for vector [0, 1, 0]
glm::quat RotationFromY(glm::vec3 dest);


class SystemTransform{
public:
    SystemTransform() {}
    SystemTransform(glm::vec3 global, glm::vec3 local)
        : global_to_local(RotationBetweenVectors(global, local)),
          //local_to_global(RotationBetweenVectors(local, global))
          local_to_global(glm::inverse(global_to_local))
    {}
    inline glm::vec3 toGlobal(glm::vec3 local) const{
        return local_to_global * local;
    }
    inline glm::vec3 toLocal(glm::vec3 global) const{
        return global_to_local * global;
    }
private:
    glm::quat global_to_local;
    glm::quat local_to_global;
};

#endif // __GLM_HPP__
