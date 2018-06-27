#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <vector>
#include <set>
#include <unordered_map>

#include "glm.hpp"
#include "primitives.hpp"
#include "texture.hpp"

struct UncompressedKdNode;
struct CompressedKdNode;

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;

class Scene{
public:
    Scene() {};
    ~Scene();

    // Copying is forbidden
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void LoadAiNode(const aiScene* scene, const aiNode* ainode, glm::mat4 transform, std::string force_mat = "");
    void LoadAiMesh(const aiScene* scene, const aiMesh* mesh, glm::mat4 transform, std::string force_mat = "");
    void LoadAiSceneMeshes(const aiScene* scene, glm::mat4 transform, std::string force_mat = "");
    void LoadAiSceneMaterials(const aiScene* scene, std::string default_brdf, std::string working_directory, bool override_materials = false);
    void RegisterMaterial(std::shared_ptr<Material> material, bool override = false);

    void AddPrimitive(const primitive_data& primitive, glm::mat4 transform, std::string material, glm::mat3 texture_transform);

    // Makes a set of thin glass material references that match any of given substrings.
    void MakeThinglassSet(std::vector<std::string>);

    // Copies the data from load buffers to optimized, contignous structures.
    void Commit();
    // Compresses the kd-tree. Called automatically by Commit()
    void Compress();

    // Prints the entire buffer to stdout.
    void Dump() const;

    // Searches for the nearest intersection in the diretion specified by ray.
    Intersection    FindIntersectKd   (const Ray& r)
        __restrict__ const __attribute__((hot));
    // Searches for any interection (not necessarily nearest). Slightly faster than FindIntersectKd.
    const Triangle* FindIntersectKdAny(const Ray& r)
        __restrict__ const __attribute__((hot));
    // Searches for the nearest intersection, but ignores all intersections with the specified ignored triangle.
    Intersection    FindIntersectKdOtherThan(const Ray& r, const Triangle* ignored)
        __restrict__ const __attribute__((hot));
    // Searches for the nearest intersection, but ignores both the specified ignored triangle, as well as all triangles
    //  that use material specified in thinglass set. However, such materials are gathered into a list (ordered) in the
    //  returned value. Useful for simulating thin colored glass.
    Intersection    FindIntersectKdOtherThanWithThinglass(const Ray& r, const Triangle* ignored)
        __restrict__ const __attribute__((hot));

    // Returns true IFF the two points are visible from each other.
    // Incorporates no cache of any kind.
    bool Visibility(glm::vec3 a, glm::vec3 b) __restrict__ const __attribute__((hot));
    bool VisibilityWithThinglass(glm::vec3 a, glm::vec3 b,
                                 /*out*/ ThinglassIsections&)
        __restrict__ const __attribute__((hot));

    // Loads a texture from file, or returns a (scene-locally) cached version
    std::shared_ptr<ReadableTexture> GetTexture(std::string path);
    // Adds a new solid color texture owned by the scene, and returns a pointer to it
    std::shared_ptr<ReadableTexture> CreateSolidTexture(Color c);

    // TODO: have triangle access these, and keep these fields private
    glm::vec3*     vertices  = nullptr;
    unsigned int n_vertices  = 0;
    Triangle*      triangles = nullptr;
    unsigned int n_triangles = 0;
    glm::vec3*     normals   = nullptr;
    unsigned int n_normals   = 0;
    glm::vec3*     tangents  = nullptr;
    unsigned int n_tangents  = 0;
    glm::vec2*     texcoords = nullptr;
    unsigned int n_texcoords = 0;

    // Point lights
    std::vector<Light> pointlights;
    void AddPointLight(Light);
    // Areal lights
    struct ArealLight{
        // TODO: Rember to sort this (descending order)
        std::vector<std::pair<float,unsigned int>> triangles_with_areas;
        mutable float total_area = 0.0f;

        Radiance emission;
        float power = 0.0f;

        Light GetRandomLight(const Scene& parent, float light_sample, glm::vec2 triangle_sample, bool debug) const;
    };
    float total_areal_power;
    float total_point_power;
    std::vector<std::pair<float,ArealLight>> areal_lights;


    Light GetRandomLight(glm::vec2 choice_sample, float light_sample, glm::vec2 triangle_sample, bool debug) const;


    // Indexed by triangles.
    std::vector<float> xevents;
    std::vector<float> yevents;
    std::vector<float> zevents;

    // The bounding box for the entire scene.
    std::pair<float,float> xBB;
    std::pair<float,float> yBB;
    std::pair<float,float> zBB;

    // Dynamically determined by examining scene's diameter
    float epsilon = 0.0001f;

    void SetSkyboxColor(Color c, float intensity){
        skybox_mode = SimpleRadiance;
        skybox_color = c;
        skybox_intensity = intensity;
    }
    void SetSkyboxEnvmap(std::string path, float intensity, float rotate){
        skybox_mode = Envmap;
        skybox_texture = GetTexture(path);
        skybox_intensity = intensity;
        skybox_rotate = rotate;
    }
    Radiance GetSkyboxRay(glm::vec3 direction, bool debug=false) const;

    std::set<std::shared_ptr<const Material>> thinglass;

    std::shared_ptr<Material> GetMaterialByName(std::string name) const;
private:

    UncompressedKdNode* uncompressed_root = nullptr;

    CompressedKdNode* compressed_array = nullptr;
    unsigned int compressed_array_size = 0;
    unsigned int* compressed_triangles = nullptr;
    unsigned int compressed_triangles_size = 0;

    void CompressRec(const UncompressedKdNode* node, unsigned int& array_pos, unsigned int& triangle_pos);

    mutable std::vector<glm::vec3> vertices_buffer;
    mutable std::vector<Triangle> triangles_buffer;
    mutable std::vector<glm::vec3> normals_buffer;
    mutable std::vector<glm::vec3> tangents_buffer;
    mutable std::vector<glm::vec2> texcoords_buffer;

    std::vector<std::shared_ptr<Material>> materials; //TODO: This vector is redundant, the map below is enough
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_by_name;
    // TODO: Material* default_material;

    // This map is the owner of all file-based textures in this scene
    std::unordered_map<std::string, std::shared_ptr<ReadableTexture>> textures;
    // This vector owns all solid-color textures in this scene.
    std::vector<std::shared_ptr<ReadableTexture>> aux_textures;

    enum SkyboxMode : int{
        SimpleRadiance,
        Envmap,
    };
    SkyboxMode skybox_mode;
    Color skybox_color;
    std::shared_ptr<ReadableTexture> skybox_texture;
    float skybox_intensity;
    float skybox_rotate;

    void FreeBuffers();
    void FreeTextures();
    void FreeMaterials();
    void FreeCompressedTree();
};


#define EMPTY_BONUS 0.5f
#define ISECT_COST 80.0f
#define TRAV_COST 2.0f

struct UncompressedKdNode{
    const Scene* parent_scene;
    enum {LEAF, INTERNAL} type = LEAF;
    unsigned int depth = 0;
    std::pair<float,float> xBB;
    std::pair<float,float> yBB;
    std::pair<float,float> zBB;
    std::vector<unsigned int> triangle_indices;

    void Subdivide(unsigned int max_depth);
    UncompressedKdNode* ch0 = nullptr;
    UncompressedKdNode* ch1 = nullptr;

    float prob0, prob1;

    int dups = 0;
    // Total triangles / leaf nodes / total nodes / total dups
    std::tuple<int, int, int, int> GetTotals() const;

    void FreeRecursivelly();

    float GetCost() const;

    int split_axis;
    float split_pos;
};

struct CompressedKdNode{
    inline bool IsLeaf() const {return (kind & 0x03) == 0x03;}
    inline short GetSplitAxis() const {return kind & 0x03;}
    inline float GetSplitPlane() const {return split_plane;}
    inline uint32_t GetTrianglesN() const {return triangles_num >> 2;}
    inline uint32_t GetFirstTrianglePos() const {return triangles_start;}
    inline uint32_t GetOtherChildIndex() const {return other_child >> 2;}

    // Default constructor;
    CompressedKdNode() {}

    // Constructor for internal nodes
    CompressedKdNode(short axis, float split) {
        kind = axis;
        split_plane = split;
    }
    // Constructor for leaf nodes
    CompressedKdNode(unsigned int num, unsigned int start){
        triangles_num = (num << 2) | 0x03;
        triangles_start = start;
    }
    // Once the other child is placed, it's position has to be set in parent node
    inline void SetOtherChild(unsigned int pos){
        other_child = (other_child & 0x03) | (pos << 2);
    }

private:
    union{
        float split_plane; // For internal nodes
        uint32_t triangles_start; // For leaf nodes
    };
    union{
         // For internal nodes (shifted right 2 bits). One child is
         // just after this stuct in memory layout, other is at this
         // location.
        uint32_t other_child;
         // For leaf nodes (shifter right 2 bits).
        uint32_t triangles_num;
        // For any kind of node, 2 LSB.
        uint32_t kind;
    };
} __attribute__((packed,aligned(4)));

#endif //__SCENE_HPP__
