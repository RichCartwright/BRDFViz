#ifndef __PRIMITITES_HPP__
#define __PRIMITITES_HPP__

#include <functional>

#include "glm.hpp"
#include "radiance.hpp"
#include "ray.hpp"
#include <memory>
#include <vector>

class BRDF;

// I have to be c++11 compatible! :(
namespace std{
    template <class T, class... Args>
    std::unique_ptr<T> make_unique(Args... a){
        return std::unique_ptr<T>(new T(a...));
    }
}

class Scene;
class ReadableTexture;

struct Light{
    enum Type{
        FULL_SPHERE,
        HEMISPHERE,
    };
    Light(Type t) : type(t) {}
    Type type;
    glm::vec3 pos;
    Radiance color;
    float intensity;
    // TODO: union?
    float size; // Only for full_sphere lights
    glm::vec3 normal; // Only for hemisphere lights
    float GetDirectionalFactor(glm::vec3 v) const{
        if(type == FULL_SPHERE) return 1.0f;
        else return glm::max(0.0f, glm::dot(v,normal));
    }
};

class Material;
/*
struct Material{
    Material();
    Material(const Material&) = default;
    std::string name;

    float exponent;
    float refraction_index;
    float translucency = 0.0f;

    std::shared_ptr<ReadableTexture> diffuse;
    std::shared_ptr<ReadableTexture> specular;
    Radiance emission;
    std::shared_ptr<ReadableTexture> bumpmap;

    std::shared_ptr<BRDF> brdf;
};
*/

class Triangle{
public:
    const Scene* parent_scene;
    unsigned int va, vb, vc; // Vertex and normal indices
    Material* mat; // Material pointer
    glm::vec4 p; // plane
    //inline glm::vec3 generic_normal() const {return p.xyz();}
    
    //This seems to fix for the new glm update - Not sure why the xyz return isnt working
    //This is just the default
    inline glm::vec3 generic_normal() const { return glm::vec3{p}; } 
    void CalculatePlane() __attribute__((hot));
    float GetArea() const;
    glm::vec3 GetRandomPoint(glm::vec2 sample) const;

    Triangle(const Scene* parent, unsigned int va, unsigned int vb, unsigned int vc, Material* mat) :
        parent_scene(parent), va(va), vb(vb), vc(vc), mat(mat) {}
    Triangle() : parent_scene(nullptr) {}

    const Material& GetMaterial() const;
    const glm::vec3 GetVertexA()  const;
    const glm::vec3 GetVertexB()  const;
    const glm::vec3 GetVertexC()  const;
    const glm::vec3 GetNormalA()  const;
    const glm::vec3 GetNormalB()  const;
    const glm::vec3 GetNormalC()  const;
    const glm::vec2 GetTexCoordsA()  const;
    const glm::vec2 GetTexCoordsB()  const;
    const glm::vec2 GetTexCoordsC()  const;
    const glm::vec3 GetTangentA()  const;
    const glm::vec3 GetTangentB()  const;
    const glm::vec3 GetTangentC()  const;

    bool TestIntersection(const Ray& r, /*out*/ float& t, float& a, float& b, bool debug = false) const __attribute__((hot));
};

typedef std::vector<std::tuple<const Triangle*,float>> ThinglassIsections;
struct Intersection{
    const Triangle* triangle = nullptr;
    float t;
    float a,b,c;
    template <typename T>
    T Interpolate(const T& x, const T& y, const T& z) {return a*x + b*y + c*z;}
    // An ordered list of intersections with materials that are considered to be a thin glass.
    // The first element of pair is the triangle intersecting. The second is the distance from ray origin
    // to the intersection. The second parameter is used because triangles may get cloned during kD-tree
    // construction, and we need to apply a filter just once.
    ThinglassIsections thinglass;
};


typedef std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec2, glm::vec3>> primitive_data;
class Primitives{
public:
    static primitive_data planeY;
    static primitive_data trigY;
    static primitive_data cube;
};

#endif // __PRIMITITES_HPP__
