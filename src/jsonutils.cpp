#include "jsonutils.hpp"
#include "config.hpp"
#include "utils.hpp"

std::string JsonUtils::fileloc_name;

bool JsonUtils::JSONToVec3(Json::Value v, glm::vec3& out){
    if(v.isArray()){
        if(v.size() != 3) return false;
        const auto& x = v[0]; if(!x.isNumeric()) return false; out.x = x.asFloat();
        const auto& y = v[1]; if(!y.isNumeric()) return false; out.y = y.asFloat();
        const auto& z = v[2]; if(!z.isNumeric()) return false; out.z = z.asFloat();
        return true;
    }else if(v.isNumeric()){
        out = glm::vec3(v.asFloat());
        return true;
    }
    return false;
}

std::string JsonUtils::getRequiredString(const Json::Value& node, std::string key) {
    if(!node.isMember(key))
        throw ConfigFileException("Required value \"" + key +"\" is missing from " + getNodeSemanticName(node) + ".");
    if(!node[key].isString())
        throw ConfigFileException("Required value \""+ key + "\" in " + getNodeSemanticName(node) + " must be a string.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return node[key].asString();
}
int JsonUtils::getRequiredInt(const Json::Value& node, std::string key) {
    if(!node.isMember(key))
        throw ConfigFileException("Required value \"" + key +"\" is missing from " + getNodeSemanticName(node) + ".");
    if(!node[key].isNumeric())
        throw ConfigFileException("Required value \""+ key + "\" in " + getNodeSemanticName(node) + " must be a number.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return node[key].asInt();
}
float JsonUtils::getRequiredFloat(const Json::Value& node, std::string key) {
    if(!node.isMember(key))
        throw ConfigFileException("Required value \"" + key +"\" is missing from  " + getNodeSemanticName(node) + ".");
    if(!node[key].isNumeric())
        throw ConfigFileException("Required value \""+ key + "\" in " + getNodeSemanticName(node) + " must be a number.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return node[key].asFloat();
}
glm::vec3 JsonUtils::getRequiredVec3(const Json::Value& node, std::string key) {
    if(!node.isMember(key))
        throw ConfigFileException("Required value \"" + key +"\" is missing from " + getNodeSemanticName(node) + ".");
    glm::vec3 res;
    if(!JSONToVec3(node[key], res))
        throw ConfigFileException("Required value \"" + key + "\" in " + getNodeSemanticName(node) + " must be an array of 3 numbers or a single number.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return res;
}
glm::vec3 JsonUtils::getRequiredVec3_255(const Json::Value& node, std::string key) {
    if(!node.isMember(key) && !node.isMember(key+"255"))
        throw ConfigFileException("Required value \"" + key +"\" is missing from " + getNodeSemanticName(node) + ".");
    glm::vec3 res;
    if(node.isMember(key)){
        if(!JSONToVec3(node[key], res))
            throw ConfigFileException("Required value \"" + key + "\" in " + getNodeSemanticName(node) + " must be an array of 3 numbers or a single number.");
        markNodeUsed(*const_cast<Json::Value*>(&node[key]));
        return res;
    }else{
        if(!JSONToVec3(node[key+"255"], res))
            throw ConfigFileException("Required value \"" + key + "255\" in " + getNodeSemanticName(node) + " must be an array of 3 numbers or a single number.");
        markNodeUsed(*const_cast<Json::Value*>(&node[key+"255"]));
        return res/255.0f;
    }
}
std::string JsonUtils::getOptionalString(const Json::Value& node, std::string key, std::string def) {
    if(!node.isMember(key)) return def;
    if(!node[key].isString())
        throw ConfigFileException("Optional value \""+ key + "\"in " + getNodeSemanticName(node) + " must be a string.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return node[key].asString();
}
int JsonUtils::getOptionalInt(const Json::Value& node, std::string key, int def) {
    if(!node.isMember(key)) return def;
    if(!node[key].isNumeric())
        throw ConfigFileException("Optional value \""+ key + "\" in " + getNodeSemanticName(node) + " must be a number.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return node[key].asInt();
}
float JsonUtils::getOptionalFloat(const Json::Value& node, std::string key, float def) {
    if(!node.isMember(key)) return def;
    if(!node[key].isNumeric())
        throw ConfigFileException("Optional value \""+ key + "\" in " + getNodeSemanticName(node) + " must be a number.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return node[key].asFloat();
}
bool JsonUtils::getOptionalBool(const Json::Value& node, std::string key, bool def) {
    if(!node.isMember(key)) return def;
    if(!node[key].isBool())
        throw ConfigFileException("Optional value \""+ key + "\" in " + getNodeSemanticName(node) + " must be a bool.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return node[key].asBool();
}
glm::vec3 JsonUtils::getOptionalVec3(const Json::Value& node, std::string key, glm::vec3 def) {
    if(!node.isMember(key)) return def;
    glm::vec3 res;
    if(!JSONToVec3(node[key], res))
        throw ConfigFileException("Optional value \"" + key + "\" in " + getNodeSemanticName(node) + " must be an array of 3 numbers or a single number.");
    markNodeUsed(*const_cast<Json::Value*>(&node[key]));
    return res;
}
glm::vec3 JsonUtils::getOptionalVec3_255(const Json::Value& node, std::string key, glm::vec3 def) {
    if(!node.isMember(key) && !node.isMember(key+"255")) return def;
    glm::vec3 res;
    if(node.isMember(key)){
        if(!JSONToVec3(node[key], res))
            throw ConfigFileException("Optional value \"" + key + "\" in " + getNodeSemanticName(node) + " must be an array of 3 numbers or a single number.");
        markNodeUsed(*const_cast<Json::Value*>(&node[key]));
        return res;
    }else{
        if(!JSONToVec3(node[key+"255"], res))
            throw ConfigFileException("Optional value \"" + key + "255\" in " + getNodeSemanticName(node) + " must be an array of 3 numbers or a single number.");
        markNodeUsed(*const_cast<Json::Value*>(&node[key+"255"]));
        return res/255.0f;
    }
}


void JsonUtils::prepareNodeMetadata(Json::Value& node, bool recursive){
    std::string metadata = Utils::JoinString({"//", "N","(unknown location)"}, "|");
    node.setComment(metadata, Json::CommentPlacement::commentAfterOnSameLine);
    if(recursive && (node.type() == Json::ValueType::arrayValue ||
                     node.type() == Json::ValueType::objectValue)){
        for(auto& child : node)
            prepareNodeMetadata(child, true);
    }
}
void JsonUtils::markNodeUsed(Json::Value& node){
    auto vs = Utils::SplitString(node.getComment(Json::CommentPlacement::commentAfterOnSameLine), "|");
    assert(vs.size() == 3);
    vs[1] = "Y";
    node.setComment(Utils::JoinString(vs, "|"), Json::CommentPlacement::commentAfterOnSameLine);
}
void JsonUtils::markNodeUnused(Json::Value& node){
    auto vs = Utils::SplitString(node.getComment(Json::CommentPlacement::commentAfterOnSameLine), "|");
    assert(vs.size() == 3);
    vs[1] = "N";
    node.setComment(Utils::JoinString(vs, "|"), Json::CommentPlacement::commentAfterOnSameLine);
}
bool JsonUtils::getNodeUsed(const Json::Value& node){
    auto vs = Utils::SplitString(node.getComment(Json::CommentPlacement::commentAfterOnSameLine), "|");
    assert(vs.size() == 3);
    return vs[1] == "Y";
}
void JsonUtils::setNodeSemanticName(Json::Value& node, std::string name){
    auto vs = Utils::SplitString(node.getComment(Json::CommentPlacement::commentAfterOnSameLine), "|");
    assert(vs.size() == 3);
    vs[2] = name;
    node.setComment(Utils::JoinString(vs, "|"), Json::CommentPlacement::commentAfterOnSameLine);
}
std::string JsonUtils::getNodeSemanticName(const Json::Value& node){
    auto vs = Utils::SplitString(node.getComment(Json::CommentPlacement::commentAfterOnSameLine), "|");
    assert(vs.size() == 3);
    return vs[2];
}


std::vector<std::string> JsonUtils::findUnusedNodes(const Json::Value& node){
    // TODO: Maybe a list for faster concatenation?
    // It's not a big deal since config files are small anyway though
    std::vector<std::string> result;
    if(node.type() == Json::ValueType::arrayValue){
        for(auto& child : node){
            auto r = findUnusedNodes(child);
            result.insert(result.end(), r.begin(), r.end());
        }
    }else if(node.type() == Json::ValueType::objectValue){
        for(auto it = node.begin(); it != node.end(); it++){
            auto r = findUnusedNodes(*it);
            result.insert(result.end(), r.begin(), r.end());
            if(!getNodeUsed(*it)){
                std::string name = getNodeSemanticName(node);
                result.push_back("key \"" + it.name() + "\" in " + name);
            }
        }
    }
    return result;
}
