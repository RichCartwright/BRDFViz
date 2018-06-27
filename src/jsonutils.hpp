#ifndef __JSONUTILS_HPP__
#define __JSONUTILS_HPP__

#include <string>

#include "../external/json/json.h"
#include "glm.hpp"

class JsonUtils{
public:
    static void SetFileLocationName(std::string name){
        fileloc_name = name;
    }

    static std::string getRequiredString(const Json::Value& node, std::string key);
    static int         getRequiredInt   (const Json::Value& node, std::string key);
    static float       getRequiredFloat (const Json::Value& node, std::string key);
    static glm::vec3   getRequiredVec3  (const Json::Value& node, std::string key);
    static glm::vec3   getRequiredVec3_255(const Json::Value& node, std::string key);

    static std::string getOptionalString(const Json::Value& node, std::string key, std::string def);
    static int         getOptionalInt   (const Json::Value& node, std::string key, int def);
    static float       getOptionalFloat (const Json::Value& node, std::string key, float def);
    static bool        getOptionalBool  (const Json::Value& node, std::string key, bool def);
    static glm::vec3   getOptionalVec3  (const Json::Value& node, std::string key, glm::vec3 def);
    static glm::vec3   getOptionalVec3_255(const Json::Value& node, std::string key, glm::vec3 def);

    static void prepareNodeMetadata(Json::Value& node, bool recursive=1);
    static void markNodeUsed(Json::Value& node);
    static void markNodeUnused(Json::Value& node);
    static bool getNodeUsed(const Json::Value& node);
    // Note: Semantic name may not contain a | character
    static void setNodeSemanticName(Json::Value& node, std::string name);
    static std::string getNodeSemanticName(const Json::Value& node);

    static std::vector< std::string > findUnusedNodes(const Json::Value& root);
private:
    static bool JSONToVec3(Json::Value v, glm::vec3& out);
    static std::string fileloc_name;
};

#endif // __JSONUTILS_HPP__
