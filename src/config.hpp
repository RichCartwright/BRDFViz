#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>
#include <vector>
#include <exception>
#include <memory>

#include "glm.hpp"
#include "primitives.hpp"
#include "camera.hpp"
#include "../external/json/json.h"

class Scene;

struct ConfigFileException : public std::runtime_error{
    ConfigFileException(const std::string& what ) : std::runtime_error(what) {}
};

enum class RenderLimitMode{
    Rounds,
    Timed,
};

class Config{
public:
    std::string config_file_path;
    FILE* dataFile;
    std::string comment;
    std::string output_file;
    unsigned int recursion_level = 40;
    unsigned int xres, yres;
    unsigned int multisample = 1;
    float bumpmap_scale = 10.0f;
    float clamp = 256.0f;
    float russian = -1.0f;
    float output_scale = -1.0f;
    RenderLimitMode render_limit_mode = RenderLimitMode::Rounds;
    unsigned int render_rounds = 1;
    unsigned int render_minutes = -1;
    bool force_fresnell = false;
    unsigned int reverse = 0;
    bool collect_data = false; 
    //std::string brdf = "cooktorr";
    std::vector<std::string> thinglass;

    virtual Camera GetCamera(float rotation) const = 0;
    virtual void InstallLights(Scene& scene) const = 0;
    virtual void  InstallScene(Scene& scene) const = 0;
    virtual void InstallMaterials(Scene& scene) const = 0;
    virtual void InstallSky(Scene& scene) const = 0;
    virtual void PerformPostCheck() const = 0;
    void CreateCSV(const char* FileName);
protected:
    Config(){};
};

class ConfigRTC : public Config{
public:
    static std::shared_ptr<ConfigRTC> CreateFromFile(std::string path);
    virtual Camera GetCamera(float rotation) const override;
    virtual void InstallLights(Scene& scene) const override;
    virtual void  InstallScene(Scene& scene) const override;
    virtual void InstallMaterials(Scene& scene) const override;
    virtual void InstallSky(Scene& scene) const override;
    virtual void PerformPostCheck() const override;
private:
    ConfigRTC(){};

    // Pre-fetching all values when the file is loaded
    std::string model_file;
    glm::vec3 camera_position;
    glm::vec3 camera_lookat;
    glm::vec3 camera_upvector;
    float yview;
    std::vector<Light> lights;
    float lens_size = 0.0f;
    float focus_plane = 1.0f;
    Color sky_color = Color(0.0, 0.0, 0.0);
    float sky_brightness = 2.0;
    std::string brdf;
};

class ConfigJSON : public Config{
public:
    static std::shared_ptr<ConfigJSON> CreateFromFile(std::string path);
    virtual Camera GetCamera(float rotation) const override;
    virtual void InstallLights(Scene& scene) const override;
    virtual void  InstallScene(Scene& scene) const override;
    virtual void InstallMaterials(Scene& scene) const override;
    virtual void InstallSky(Scene& scene) const override;
    virtual void PerformPostCheck() const override;
private:
    ConfigJSON(){};

    mutable Json::Value root;
};

#endif // __CONFIG_HPP__
