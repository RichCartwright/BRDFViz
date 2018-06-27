#include "glm.hpp"

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
    start = normalize(start);
    dest = normalize(dest);

    float cosTheta = dot(start, dest);
    glm::vec3 rotationAxis;

    if (cosTheta < -1 + 0.001f){
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), start);
        if (glm::length(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

        rotationAxis = normalize(rotationAxis);
        //std::cout << "Rotation axis = " << rotationAxis << std::endl;
        return glm::angleAxis(glm::pi<float>(), rotationAxis);
    }

    rotationAxis = cross(start, dest);

    float s = sqrt( (1+cosTheta)*2 );
    float invs = 1 / s;

    return glm::quat(s * 0.5f,
                     rotationAxis.x * invs,
                     rotationAxis.y * invs,
                     rotationAxis.z * invs
                     );
}
// Same as above, optimized for vector [0, 1, 0]
glm::quat RotationFromY(glm::vec3 dest){
    dest = normalize(dest);

    float cosTheta = dest.y;
    glm::vec3 rotationAxis;

    if (cosTheta < -1 + 0.00001f){
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = glm::vec3(1.0,0.0,0.0);
        return glm::angleAxis(glm::pi<float>(), rotationAxis);
    }

    rotationAxis = glm::cross(glm::vec3(0.0, 1.0, 0.0), dest);

    float s = sqrt( (1+cosTheta)*2 );
    float invs = 1 / s;

    return glm::quat(s * 0.5f,
                     rotationAxis.x * invs,
                     rotationAxis.y * invs,
                     rotationAxis.z * invs
                     );
}
