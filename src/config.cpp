#include "config.hpp"

#include <fstream>
#include <cmath>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "utils.hpp"
#include "scene.hpp"
#include "out.hpp"
#include "bxdf/bxdf.hpp"
#include "jsonutils.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#define NEXT_LINE()                                                                  \
    do{ std::getline(file, line);                                                    \
        line = Utils::Trim(line);                                                    \
        if(!file.good()) throw ConfigFileException("Config file ends prematurely."); \
    }while(0)

void Config::CreateCSV(const char* FileName)
{
	//TODO: Suppoort multiple csv's for camera movement "animation"
	/*std::stringsteam ss(std::stringstream::in | std::stringstream::out); 
	ss << "./data/data.csv";
	dataFile = fopen(ss.str().c_str(), "w");*/
	dataFile = fopen(FileName, "w");
	if(!dataFile) 
	{
		std::cout << "There has been an error creating the data CSV. Data will not be collected" << 									"\n";
		collect_data = false;
		return; 
	}
	std::cout << "Successfully created " << FileName << "\n";
}

std::shared_ptr<ConfigRTC> ConfigRTC::CreateFromFile(std::string path){
    auto cfgptr = std::shared_ptr<ConfigRTC>(new ConfigRTC());
    ConfigRTC& cfg = *cfgptr;
    cfg.config_file_path = path;

    std::ifstream file(path, std::ios::in);
    if(!file) throw ConfigFileException("Failed to open config file ` " + path + " `.");

    std::string line;
    std::vector<std::string> vs;

    std::getline(file, line);  // comment
    line = Utils::Trim(line);
    cfg.comment = line;
    NEXT_LINE();  // model file
    cfg.model_file = line;
    NEXT_LINE();  // output file
    cfg.output_file = line;
    NEXT_LINE();  // recursion level
    unsigned int k = std::stoi(line);
    // if(k==0) throw ConfigFileException("Invalid k value.");
    cfg.recursion_level = k;
    NEXT_LINE(); // xres yres

    vs = Utils::SplitString(line," ");
    if(vs.size()!=2) throw ConfigFileException("Invalid resolution format.");
    unsigned int xres = std::stoi(vs[0]), yres = std::stoi(vs[1]);
    if(xres==0 || yres==0) throw ConfigFileException("Invalid output image resolution.");
    cfg.xres = xres;
    cfg.yres = yres;

    NEXT_LINE(); // VP
    vs = Utils::SplitString(line," ");
    if(vs.size()!=3) throw ConfigFileException("Invalid VP format.");
    float VPx = std::stof(vs[0]), VPy = std::stof(vs[1]), VPz = std::stof(vs[2]);
    cfg.camera_position = glm::vec3(VPx, VPy, VPz);

    NEXT_LINE(); // LA
    vs = Utils::SplitString(line," ");
    if(vs.size()!=3) throw ConfigFileException("Invalid LA format.");
    float LAx = std::stof(vs[0]), LAy = std::stof(vs[1]), LAz = std::stof(vs[2]);
    // TODO: What it LA - VP = 0 ?
    cfg.camera_lookat = glm::vec3(LAx, LAy, LAz);

    NEXT_LINE(); // UP
    vs = Utils::SplitString(line," ");
    if(vs.size()!=3) throw ConfigFileException("Invalid UP format.");
    float UPx = std::stof(vs[0]), UPy = std::stof(vs[1]), UPz = std::stof(vs[2]);
    cfg.camera_upvector = glm::vec3(UPx, UPy, UPz);

    NEXT_LINE(); // yview
    float yview = std::stof(line);
    if(yview <= 0.0f || yview >= 100.0f) throw ConfigFileException("Invalid yview value.");
    cfg.yview = yview;

    // Now for the extra lines
    line = "";
    do{
        std::getline(file, line);
        line = Utils::Trim(line);
        vs = Utils::SplitString(line," ");
        if(vs.size() == 0) continue;
        if(vs[0][0] == '#') continue;
        if(vs[0] == "") continue;
        if(vs[0] == "L"){
            // Light info
            if(vs.size() < 8 || vs.size() > 9) throw ConfigFileException("Invalid light line.");
            float l1 = std::stof(vs[1]), l2 = std::stof(vs[2]), l3 = std::stof(vs[3]);
            float c1 = std::stof(vs[4])/255, c2 = std::stof(vs[5])/255, c3 = std::stof(vs[6])/255;
            float i = std::stof(vs[7]);
            float s = 0.0f;
            if(vs.size() == 9) s = std::stof(vs[8]);
            Light l(Light::Type::FULL_SPHERE);
            l.pos = glm::vec3(l1,l2,l3);
            l.color = Radiance(c1,c2,c3);
            l.size = s;
            l.intensity = i;
            cfg.lights.push_back(l);
        }else if(vs[0] == "multisample" || vs[0] == "ms"){
            if(vs.size() != 2) throw ConfigFileException("Invalid multisample line.");
            int ms = std::stoi(vs[1]);
            if(ms == 0) throw ConfigFileException("Invalid multisample value.");
            cfg.multisample = ms;
        }else if(vs[0] == "sky" || vs[0] == "skycolor"){
            if(vs.size() < 4 || vs.size() > 5) throw ConfigFileException("Invalid sky color line.");
            cfg.sky_color = Color(std::stoi(vs[1])/255.0f, std::stoi(vs[2])/255.0f, std::stoi(vs[3])/255.0f);
            if(vs.size() == 5) cfg.sky_brightness = std::stof(vs[4]);
        }else if(vs[0] == "lens" || vs[0] == "lenssize" || vs[0] == "lens_size"){
            if(vs.size() != 2) throw ConfigFileException("Invalid lens size line.");
            float ls = std::stof(vs[1]);
            if(ls < 0) throw ConfigFileException("Lens size must be a poositive value.");
            cfg.lens_size = ls;
        }else if(vs[0] == "focus" || vs[0] == "focus_plane" || vs[0] == "focus_dist"){
            if(vs.size() != 2) throw ConfigFileException("Invalid focus plane line.");
            float fp = std::stof(vs[1]);
            if(fp < 0) throw ConfigFileException("Focus plane must be a poositive value.");
            cfg.focus_plane = fp;
        }else if(vs[0] == "bump_scale" || vs[0] == "bumpmap_scale" || vs[0] == "bump" || vs[0] == "bumpscale"){
            if(vs.size() != 2) throw ConfigFileException("Invalid bump scale config line.");
            float bs = std::stof(vs[1]);
            cfg.bumpmap_scale = bs;
        }else if(vs[0] == "clamp"){
            if(vs.size() != 2) throw ConfigFileException("Invalid clamp config line.");
            cfg.clamp = std::stof(vs[1]);
        }else if(vs[0] == "russian" || vs[0] == "roulette"){
            if(vs.size() != 2) throw ConfigFileException("Invalid russian roulette config line.");
            cfg.russian = std::stof(vs[1]);
        }else if(vs[0] == "rounds"){
            if(vs.size() != 2) throw ConfigFileException("Invalid rounds config line.");
            cfg.render_rounds = std::stoi(vs[1]);
        }else if(vs[0] == "reverse"){
            if(vs.size() != 2) throw ConfigFileException("Invalid reverse config line.");
            cfg.reverse = std::stoi(vs[1]);
        }else if(vs[0] == "brdf"){
            if(vs.size() != 2) throw ConfigFileException("Invalid brdf config line.");
            if(vs[1] == "cooktorr"){
                cfg.brdf = "cooktorr";
            }else if(vs[1] == "phong"){
                cfg.brdf = "phong";
            }else if(vs[1] == "phong2"){
                cfg.brdf = "phong2";
            }else if(vs[1] == "phongenergy"){
                cfg.brdf = "phongenergy";
            }else if(vs[1] == "diffuse"){
                cfg.brdf = "diffusecosine";
            }else if(vs[1] == "diffuseuniform"){
                cfg.brdf = "diffuseuniform";
            }else if(vs[1] == "ltc_beckmann"){
                cfg.brdf = "ltc_beckmann";
            }else if(vs[1] == "ltc_ggx"){
                cfg.brdf = "ltc_ggx";
            }else{
                throw ConfigFileException("Unknown BRDF type: " + vs[1]);
            }
        }else if(vs[0] == "thinglass"){
            if(vs.size() != 2) throw ConfigFileException("Invalid thinglass config line.");
            cfg.thinglass.push_back(vs[1]);
        }else if(vs[0] == "force_fresnell"){
            if(vs.size() != 2) throw ConfigFileException("Invalid force_fresnell config line.");
            cfg.force_fresnell = (std::stoi(vs[1]) == 1);
        }else{
            std::cout << "WARNING: Unrecognized option `" << vs[0] << "` in the config file." << std::endl;
        }
    }while (file.good());

    return cfgptr;
}


Camera ConfigRTC::GetCamera(float rotation) const{
    glm::vec3 p = camera_lookat - camera_position;
    p = glm::rotate(p, rotation * 2.0f * glm::pi<float>(), camera_upvector);
    glm::vec3 pos = camera_lookat - p;
    return Camera(pos,
                  camera_lookat,
                  camera_upvector,
                  yview,
                  yview*xres/yres,
                  xres,
                  yres,
                  focus_plane,
                  lens_size
                  );
}

void ConfigRTC::InstallLights(Scene& scene) const{
    for(const Light& l : lights)
        scene.AddPointLight(l);
}

static const aiScene* loadAssimpScene(Assimp::Importer& importer, std::string modelfile, bool smooth_normals = false){
    out::cout(2) << "Adding meshes from file \"" << modelfile << "\"..."  << std::flush;
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    const aiScene* scene = importer.ReadFile(modelfile,
                                             aiProcess_Triangulate |
                                             //aiProcess_TransformUVCoords |

                                             // Neither of these work correctly.
                                             ((smooth_normals)?aiProcess_GenSmoothNormals:aiProcess_GenNormals) |

                                             aiProcess_JoinIdenticalVertices |

                                             //aiProcess_RemoveRedundantMaterials |

                                             aiProcess_GenUVCoords |
                                             //aiProcess_SortByPType |
                                             aiProcess_FindDegenerates |
                                             // DO NOT ENABLE THIS CLEARLY BUGGED SEE SIBENIK MODEL aiProcess_FindInvalidData |
                                             //aiProcess_ValidateDataStructure |
                                             0 );

    if(!scene) throw ConfigFileException("Assimp failed to load scene \"" + modelfile + "\": " + importer.GetErrorString());

    // Calculating tangents is requested AFTER the scene is
    // loaded. Otherwise this step runs before normals are calculated
    // for vertices that are missing them.
    scene = importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);

    out::cout(2) << " found " << scene->mNumMeshes << " meshes and " <<
        scene->mNumMaterials << " materials." << std::endl;

    return scene;
}

void ConfigRTC::InstallScene(Scene& s) const{
    std::string configdir = Utils::GetDir(config_file_path);
    std::string modelfile = configdir + "/" + model_file;
    std::string modeldir  = Utils::GetDir(modelfile);
    if(!Utils::GetFileExists(modelfile))
        throw ConfigFileException("Unable to find model file \"" + modelfile + "\"");

    // Load the model with assimp
    Assimp::Importer importer;
    const aiScene* scene = loadAssimpScene(importer, modelfile);

    s.LoadAiSceneMaterials(scene, brdf, modeldir + "/");
    s.LoadAiSceneMeshes(scene, glm::mat4());
}

void ConfigRTC::InstallMaterials(Scene&) const{
    // Nothing to do, materials will be installed during scene installation
}

void ConfigRTC::InstallSky(Scene& s) const{
    s.SetSkyboxColor(sky_color, sky_brightness);
}

void ConfigRTC::PerformPostCheck() const{
    // nop
}

// ----- JSON ----


std::shared_ptr<ConfigJSON> ConfigJSON::CreateFromFile(std::string path){
    auto cfgptr = std::shared_ptr<ConfigJSON>(new ConfigJSON());
    ConfigJSON& cfg = *cfgptr;
    cfg.config_file_path = path;
    Json::Value& root = cfg.root;

    Json::Reader reader;

    std::ifstream file(path, std::ios::in);
    if(!file.good()) throw ConfigFileException("Failed to open file: " + path);

    reader.parse(file, root, false);
    if(!reader.good()) throw ConfigFileException("Failed to parse JSON contents: " + reader.getFormattedErrorMessages());

    JsonUtils::prepareNodeMetadata(root, true);
    JsonUtils::setNodeSemanticName(root, "the config file");

    cfg.output_file = JsonUtils::getRequiredString(root,"output-file");

    cfg.xres = JsonUtils::getRequiredInt(root,"output-width");
    cfg.yres = JsonUtils::getRequiredInt(root,"output-height");

    if(root.isMember("rounds") && root.isMember("render-time")){
        throw ConfigFileException("The config file may not contain both \"rounds\" and \"render-time\" keys simultaneously.");
    }else if(root.isMember("rounds")){
        cfg.render_limit_mode = RenderLimitMode::Rounds;
        cfg.render_rounds =  JsonUtils::getRequiredInt(root, "rounds");
    }else if(root.isMember("render-time")){
        cfg.render_limit_mode = RenderLimitMode::Timed;
        cfg.render_minutes = JsonUtils::getRequiredInt(root, "render-time");
    }else{
        cfg.render_limit_mode = RenderLimitMode::Rounds;
        cfg.render_rounds = 1;
    }

    cfg.recursion_level = JsonUtils::getOptionalInt(root, "recursion-max", 40);
    cfg.multisample =     JsonUtils::getOptionalInt(root, "multisample", 1);
    cfg.clamp =           JsonUtils::getOptionalFloat(root, "clamp", 10000000.0f);
    cfg.bumpmap_scale =   JsonUtils::getOptionalFloat(root, "bumpscale", 1.0f);
    cfg.russian =         JsonUtils::getOptionalFloat(root, "russian", 0.74f);
    cfg.reverse =         JsonUtils::getOptionalInt(root, "reverse", 0);
    cfg.force_fresnell =  JsonUtils::getOptionalBool(root, "force-fresnell", false);

    if(root.isMember("output-scale")){
        JsonUtils::markNodeUsed(root["output-scale"]);
        if(root["output-scale"].isString()){
            if(root["output-scale"].asString() == "auto"){
                cfg.output_scale = -1.0;
            }else throw ConfigFileException("The value of \"output-scale\" must either be a number, or \"auto\".");
        }else if(root["output-scale"].isNumeric()){
            cfg.output_scale = root["output-scale"].asFloat();
        }else throw ConfigFileException("The value of \"output-scale\" must either be a number, or \"auto\".");
    }

    if(root.isMember("thinglass")){
        auto thinglass = root["thinglass"];
        JsonUtils::markNodeUsed(thinglass);
        if(!thinglass.isArray()) throw ConfigFileException("Value \"thinglass\" must be an array of strings");
        for(unsigned int i = 0; i < thinglass.size(); i++){
            auto t = thinglass[i];
            if(!t.isString()) throw ConfigFileException("Value \"thinglass\" must be an array of strings");
            cfg.thinglass.push_back(t.asString());
        }
    }

    return cfgptr;
}

static float fov2xview(float fov){
    return 2.0f*std::tan(fov*0.0174533f/2.0f);
}

Camera ConfigJSON::GetCamera(float rotation) const{
    if(!root.isMember("camera")) throw ConfigFileException("Value \"camera\" is missing.");
    auto& camera = root["camera"];
    JsonUtils::markNodeUsed(camera);
    JsonUtils::setNodeSemanticName(camera, "camera configuration");
    if(!camera.isObject()) throw ConfigFileException("Value \"camera\" is not a dictionary.");

    glm::vec3 camera_position = JsonUtils::getRequiredVec3(camera, "position");
    glm::vec3 camera_lookat   = JsonUtils::getRequiredVec3(camera, "lookat"  );
    glm::vec3 camera_upvector = JsonUtils::getOptionalVec3(camera, "upvector", glm::vec3(0.0f, 1.0f, 0.0f));

    float yview, xview;
    if(camera.isMember("focal")){
        yview = JsonUtils::getRequiredFloat(camera, "focal");
        xview = yview*xres/yres;
    }else if(camera.isMember("fov")){
        xview = fov2xview(JsonUtils::getRequiredFloat(camera, "fov"));
        yview = xview*yres/xres;
    }else{
        throw ConfigFileException("Camera must either have a \"fov\" or \"focal\" key defined");
    }

    float focus_plane = JsonUtils::getOptionalFloat(camera, "focus-plane", 1.0f);
    float lens_size   = JsonUtils::getOptionalFloat(camera, "lens-size"  , 0.0f);

    glm::vec3 p = camera_lookat - camera_position;
   // p = glm::rotate(p, rotation * 2.0f * glm::pi<float>(), camera_upvector);
    p = glm::rotate(p, rotation, camera_upvector);
    glm::vec3 pos = camera_lookat - p;
    return Camera(pos,
                  camera_lookat,
                  camera_upvector,
                  yview,
                  xview,
                  xres,
                  yres,
                  focus_plane,
                  lens_size
                  );
}

void ConfigJSON::InstallLights(Scene &scene) const{
    if(!root.isMember("lights")) return; // no lights!
    auto& lights = root["lights"];
    JsonUtils::markNodeUsed(lights);
    if(!lights.isArray()) throw ConfigFileException("Value \"lights\" must be an array.");
    for(unsigned int i = 0; i < lights.size(); i++){
        auto& light = lights[i];
        JsonUtils::setNodeSemanticName(light, "light " + std::to_string(i) + " configuration");
        Light l(Light::Type::FULL_SPHERE);
        l.pos = JsonUtils::getRequiredVec3(light, "position");
        l.color = Radiance(JsonUtils::getOptionalVec3_255(light, "color", glm::vec3(1.0f, 1.0f, 1.0f)));
        l.intensity = JsonUtils::getRequiredFloat(light, "intensity");
        l.size = JsonUtils::getOptionalFloat(light, "size", 0.0f);
        scene.AddPointLight(l);
    }
}

void ConfigJSON::InstallSky(Scene& s) const{
    if(!root.isMember("sky")){
        s.SetSkyboxColor(Color(0.0f, 0.0f, 0.0f), 1.0f);
        return;
    }
    auto& sky = root["sky"];
    JsonUtils::markNodeUsed(sky);
    JsonUtils::setNodeSemanticName(sky, "sky configuration");
    if(!sky.isObject()) throw ConfigFileException("Value \"sky\" must be a dictionary.");

    if(sky.isMember("envmap")){
        std::string configdir = Utils::GetDir(config_file_path);
        std::string path = JsonUtils::getRequiredString(sky, "envmap");
        float intensity = JsonUtils::getOptionalFloat(sky, "intensity", 1.0f);
        float rotate = JsonUtils::getOptionalFloat(sky, "rotate", 0.0f);
        s.SetSkyboxEnvmap(configdir + "/" + path, intensity, rotate);
    }else if(sky.isMember("color") || sky.isMember("color255")){
        Color color = JsonUtils::getRequiredVec3_255(sky, "color");
        float intensity = JsonUtils::getOptionalFloat(sky, "intensity", 1.0f);
        s.SetSkyboxColor(color, intensity);
    }else{
        throw ConfigFileException("Sky configuration must either contain an \"envmap\" key or a \"color\" key");
    }
}


void ConfigJSON::InstallScene(Scene& s) const{
    std::string configdir = Utils::GetDir(config_file_path);
    if(root.isMember("model-file") && root.isMember("scene"))
        throw ConfigFileException("The input file may not contain both \"model-file\" key and \"scene\" key, maximum one of these is allowed.");
    if(root.isMember("model-file")){
        std::string modelfile = configdir + "/" + JsonUtils::getRequiredString(root,"model-file");
        std::string modeldir  = Utils::GetDir(modelfile);
        if(!Utils::GetFileExists(modelfile))
            throw ConfigFileException("Unable to open model file \"" + modelfile + "\"");

        // Load the model with assimp
        Assimp::Importer importer;
        const aiScene* scene = loadAssimpScene(importer, modelfile);

        std::string brdf = JsonUtils::getOptionalString(root,"brdf","ltc_ggx");

        // Do not override materials in this mode
        s.LoadAiSceneMaterials(scene, brdf, modeldir + "/", false);
        s.LoadAiSceneMeshes(scene,glm::mat4());
    }else if(root.isMember("scene")){
        auto& scene_node = root["scene"];
        JsonUtils::markNodeUsed(scene_node);
        if(!scene_node.isArray()) throw ConfigFileException("The value of \"scene\" key must be an array of objects");
        for(unsigned int i = 0; i < scene_node.size(); i++){
            auto& object = scene_node[i];
            JsonUtils::setNodeSemanticName(object, "scene object " + std::to_string(i) + " configuration");

            if(object.isMember("file") && object.isMember("primitive")){
                throw ConfigFileException("Both \"file\" and \"primitive\" keys found in " +
                                          JsonUtils::getNodeSemanticName(object) +
                                          ", only one can be present at a time.");
            }else if(object.isMember("file")){
                std::string model_file = JsonUtils::getRequiredString(object, "file");
                bool import_materials = JsonUtils::getOptionalBool(object, "import-materials", false);
                bool override_materials = JsonUtils::getOptionalBool(object, "override-materials", false);
                std::string forced_material = JsonUtils::getOptionalString(object, "material", "");

                std::string modelfile = configdir + "/" + model_file;
                std::string modeldir  = Utils::GetDir(modelfile);
                if(!Utils::GetFileExists(modelfile))
                    throw ConfigFileException("Unable to find model file \"" + modelfile + "\"");

                bool smooth_normals = JsonUtils::getOptionalBool(object, "smooth-normals", false);

                // Load the model with assimp
                Assimp::Importer importer;
                const aiScene* scene = loadAssimpScene(importer, modelfile, smooth_normals);

                std::string brdf = JsonUtils::getOptionalString(object,"brdf",
                                                                JsonUtils::getOptionalString(root,"brdf","ltc_ggx")
                                                                );

                // Get transformation details
                glm::mat4 transform;
                glm::vec3 scale = JsonUtils::getOptionalVec3(object, "scale", glm::vec3(1.0, 1.0, 1.0));
                glm::vec3 translate = JsonUtils::getOptionalVec3(object, "translate", glm::vec3(0.0, 0.0, 0.0));
                glm::vec3 rotate = JsonUtils::getOptionalVec3(object, "rotate", glm::vec3(0.0, 0.0, 0.0));
                // 1. Scale
                transform = glm::scale(scale) * transform;
                // 2. Rotation
                transform = glm::rotate(0.0174533f * rotate.z, glm::vec3(0.0, 0.0, -1.0f)) * transform;
                transform = glm::rotate(0.0174533f * rotate.y, glm::vec3(0.0, -1.0, 0.0f)) * transform;
                transform = glm::rotate(0.0174533f * rotate.x, glm::vec3(-1.0, 0.0, 0.0f)) * transform;
                // 3. Translation
                transform = glm::translate(translate) * transform;

                if(import_materials)
                    s.LoadAiSceneMaterials(scene, brdf, modeldir + "/", override_materials);

                s.LoadAiSceneMeshes(scene, transform, forced_material);

            }else if(object.isMember("primitive")){
                std::string type = JsonUtils::getRequiredString(object, "primitive");
                glm::mat4 transform;
                glm::mat3 texture_transform;
                primitive_data* data = nullptr;
                if(type == "plane"){
                    data = &Primitives::planeY;
                }else if(type == "tri"){
                    data = &Primitives::trigY;
                }else if(type == "cube"){
                    data = &Primitives::cube;
                    transform = glm::scale(glm::vec3(0.5)) * transform;
                }else{
                    throw ConfigFileException("Value \"primitive\" in " +
                                              JsonUtils::getNodeSemanticName(object) +
                                              " must be either 'cube' or 'plane'.");
                }
                std::string axis = JsonUtils::getOptionalString(object, "axis", "Y");
                if(axis == "Y"){
                    // nop
                }else if(axis == "X"){
                    transform = glm::rotate(glm::pi<float>()/2.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * transform;
                }else if(axis == "Z"){
                    transform = glm::rotate(glm::pi<float>()/2.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * transform;
                }else throw ConfigFileException("Optional value \"axis\" in " +
                                                JsonUtils::getNodeSemanticName(object) +
                                                " must be either X, Y or Z.");
                glm::vec3 scale = JsonUtils::getOptionalVec3(object, "scale", glm::vec3(1.0, 1.0, 1.0));
                glm::vec3 translate = JsonUtils::getOptionalVec3(object, "translate", glm::vec3(0.0, 0.0, 0.0));
                glm::vec3 rotate = JsonUtils::getOptionalVec3(object, "rotate", glm::vec3(0.0, 0.0, 0.0));
                // 1. Scale
                transform = glm::scale(scale) * transform;
                // 2. Rotation
                transform = glm::rotate(0.0174533f * rotate.z, glm::vec3(0.0, 0.0, -1.0f)) * transform;
                transform = glm::rotate(0.0174533f * rotate.y, glm::vec3(0.0, -1.0, 0.0f)) * transform;
                transform = glm::rotate(0.0174533f * rotate.x, glm::vec3(-1.0, 0.0, 0.0f)) * transform;
                // 3. Translation
                transform = glm::translate(translate) * transform;

                glm::vec3 texscale = JsonUtils::getOptionalVec3(object, "texture-scale", glm::vec3(1.0, 1.0, 1.0));
                texture_transform = glm::mat3(glm::scale(texscale)) * texture_transform;

                std::string material = JsonUtils::getRequiredString(object, "material");

                s.AddPrimitive(*data, transform, material, texture_transform);

                out::cout(2) << "Added a primitive with " << data->size()/3 << " faces." << std::endl;

            }else{
                throw ConfigFileException("Missing mesh data in " +
                                          JsonUtils::getNodeSemanticName(object) +
                                          ", it must either contain a \"file\" key, or \"primitive\" key.");
            }
        }
    }else{
        throw ConfigFileException("The input file contains neither \"scene\" nor \"model-file\" key.");
    }
}

void ConfigJSON::InstallMaterials(Scene& s) const{
    if(!root.isMember("materials")) return;
    auto& materials = root["materials"];
    JsonUtils::markNodeUsed(materials);
    std::string configdir = Utils::GetDir(config_file_path);
    if(!materials.isArray()) throw ConfigFileException("The value of \"materials\" key must be an array of material data");
    for(unsigned int i = 0; i < materials.size(); i++){
        auto& m = materials[i];
        JsonUtils::setNodeSemanticName(m, "material " + std::to_string(i) + " configuration");
        auto mat = std::make_shared<Material>();
        mat->LoadFromJson(m, s, configdir);
        s.RegisterMaterial(mat, true);
    }
}

void ConfigJSON::PerformPostCheck() const{
    auto unused_nodes = JsonUtils::findUnusedNodes(root);
    if(unused_nodes.empty()) return;
    out::cout(2) << "WARNING: Following configuration values are present in the config file," << std::endl;
    out::cout(2) << "but were not used when loading the file. Please check them for typos." << std::endl;
    out::cout(2) << "It is recommended to remove them from the config file." << std::endl;
    for(const std::string& str : unused_nodes){
        out::cout(2) << "    " << str << std::endl;
    }

}
