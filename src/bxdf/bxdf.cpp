#include "bxdf.hpp"
#include "../scene.hpp"
#include "../random_utils.hpp"
#include "../jsonutils.hpp"
#include "../config.hpp"
#include "../out.hpp"
#include "../utils.hpp"
#include <iostream>

#include <assimp/scene.h>

float Fresnel(glm::vec3 I, glm::vec3 N, float ior){
    float cosi = glm::dot(I, N);
    float etai = 1.0f, etat = ior;
    if (cosi > 0) { std::swap(etai, etat); }
    // Snell's law
    float sint = etai / etat * glm::sqrt(glm::max(0.f, 1.0f - cosi * cosi));
    if(sint >= 1){
        // Total internal reflection
        return 1.0f;
    }else{
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2.0f;
    }
}

glm::vec3 Refract(glm::vec3 in, glm::vec3 N, float IOR, bool debug = false){
    (void)debug;
    if(glm::dot(in, N) > 0.999f) return -in;
    glm::vec3 tangent = glm::normalize(glm::cross(N,in));
    float cosEta1 = glm::dot(in,N);
    float sinEta1 = glm::sqrt(1.0f - cosEta1 * cosEta1);
    IFDEBUG std::cout << "Eta1 " << glm::degrees(glm::angle(N, in)) << std::endl;
    IFDEBUG std::cout << "sinEta1 " << sinEta1 << std::endl;
    float sinEta2 = sinEta1 * IOR;
    IFDEBUG std::cout << "sinEta2 " << sinEta2 << std::endl;
    if(sinEta2 >= 1.0f) return glm::vec3(std::numeric_limits<float>::quiet_NaN()); // total internal
    float Eta2 = glm::asin(sinEta2);
    IFDEBUG std::cout << "Eta2 " << glm::degrees(Eta2) << std::endl;
    return glm::rotate(-N, Eta2, tangent);
}

Material::Material(){
    bxdf = nullptr;
    emission = Radiance(0,0,0);
    bumpmap  = std::make_shared<EmptyTexture>();
}

void Material::LoadFromJson(Json::Value& node, Scene& scene, std::string texturedir){
    name = JsonUtils::getRequiredString(node, "name");

    emission = Radiance(JsonUtils::getOptionalVec3_255(node, "emission", glm::vec3(0.0)));

    std::string texfile;
    texfile = JsonUtils::getOptionalString(node,"bump-map","");
    if(texfile != "") bumpmap = scene.GetTexture(texturedir + "/" + texfile);

    no_russian = JsonUtils::getOptionalBool(node, "no-russian", false);

    std::string brdf = JsonUtils::getRequiredString(node,"brdf");
    if(brdf == "diffusecosine" || brdf == "diffuse"){
        bxdf = std::make_unique<BxDFDiffuse>();
    }else if(brdf == "mix"){
        bxdf = std::make_unique<BxDFMix>();
    }else if(brdf == "dielectric"){
        bxdf = std::make_unique<BxDFDielectric>();
    }else if(brdf == "mirror"){
        bxdf = std::make_unique<BxDFMirror>();
    }else if(brdf == "transparent"){
        bxdf = std::make_unique<BxDFTransparent>();
    }else if(brdf == "ltc_beckmann"){
        bxdf = std::make_unique<BxDFLTC<LTC::Beckmann>>();
    }else if(brdf == "ltc_ggx"){
        bxdf = std::make_unique<BxDFLTC<LTC::GGX>>();
    }else if(brdf == "ltc_beckmann_diffuse"){
        bxdf = std::make_unique<BxDFLTCDiffuse<LTC::Beckmann>>();
    }else if(brdf == "ltc_ggx_diffuse"){
        bxdf = std::make_unique<BxDFLTCDiffuse<LTC::GGX>>();
    }else{
        throw ConfigFileException("Unsupported BRDF id in config!");
    }
    bxdf->LoadFromJson(node, scene, texturedir);
}

void Material::LoadFromAiMaterial(const aiMaterial* mat, Scene& scene, std::string texture_directory){

    aiString ainame;
    mat->Get(AI_MATKEY_NAME, ainame);
    name = ainame.C_Str();
    aiColor3D c;
    mat->Get(AI_MATKEY_COLOR_DIFFUSE,c);
    //std::cout << "Processing ai material named " << name << std::endl;
    auto diffuse = scene.CreateSolidTexture(Color(c.r, c.g, c.b));
    //std::cout << "Example diffuse: " << diffuse->Get(glm::vec2(0,0)) << std::endl;
    mat->Get(AI_MATKEY_COLOR_SPECULAR,c);
    auto specular = scene.CreateSolidTexture(Color(c.r, c.g, c.b));
    //std::cout << "Example specular: " << specular->Get(glm::vec2(0,0)) << std::endl;
    mat->Get(AI_MATKEY_COLOR_EMISSIVE,c);
    emission = Radiance(c.r, c.g, c.b);
    float f;

    /* TODO: Incorporate this information into bxdf choice
    mat->Get(AI_MATKEY_SHININESS, f);
    m.exponent = f/4; // This is weird. Why does assimp multiply by 4 in the first place?
    mat->Get(AI_MATKEY_REFRACTI, f);
    m.refraction_index = f;
    mat->Get(AI_MATKEY_OPACITY, f);
    m.translucency = 1.0f - f;
    */

    aiString as; std::string s;
    int n;
    n = mat->GetTextureCount(aiTextureType_DIFFUSE);
    if(n > 0){
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &as); s = as.C_Str();
        if(s != ""){
            out::cout(5) << "Material has diffuse texture " << s << std::endl;
            diffuse = scene.GetTexture(texture_directory + s);
        }
    }
    n = mat->GetTextureCount(aiTextureType_SPECULAR);
    if(n > 0){
        mat->GetTexture(aiTextureType_SPECULAR, 0, &as); s = as.C_Str();
        if(s != ""){
            out::cout(5) << "Material has specular texture " << s << std::endl;
            specular = scene.GetTexture(texture_directory + s);
        }
    }
    n = mat->GetTextureCount(aiTextureType_HEIGHT);
    if(n > 0){
        mat->GetTexture(aiTextureType_HEIGHT, 0, &as); s = as.C_Str();
        if(s != ""){
            out::cout(5) << "Material has bump texture " << s << std::endl;
            bumpmap = scene.GetTexture(texture_directory + s);
        }
    }

    mat->Get(AI_MATKEY_SHININESS, f);
    float phong_exp = f/4; // This is weird. Why does assimp multiply by 4 in the first place?
    float roughness = glm::pow(2.0f / (2.0f + phong_exp), 0.5f);

    /*
    auto diffusematerial = std::make_shared<Material>();
    auto diffusebxdf = std::make_unique<BxDFDiffuse>();
    diffusebxdf->diffuse = diffuse;
    diffusematerial->bxdf = std::move(diffusebxdf);
    diffusematerial->name = name + "|___diffuse";
    scene.RegisterMaterial(diffusematerial);

    auto specularmaterial = std::make_shared<Material>();
    auto specularbxdf = std::make_unique<BxDFLTC<LTC::GGX>>();
    specularbxdf->color = specular;
    specularbxdf->roughness = roughness;
    //std::cout << "Roughness: " << roughness << std::endl;
    specularmaterial->bxdf = std::move(specularbxdf);
    specularmaterial->name = name + "|___specular";
    scene.RegisterMaterial(specularmaterial);

    auto mixbxdf = std::make_unique<BxDFMix>();
    mixbxdf->m1 = diffusematerial;
    mixbxdf->m2 = specularmaterial;

    // TODO: Actually calculate average
    auto average_diffuse = diffuse->Get(glm::vec2(0.5, 0.5));
    auto average_specular = specular->Get(glm::vec2(0.5, 0.5));
    float diffuse_power = average_diffuse.r + average_diffuse.g + average_diffuse.b;
    float specular_power = average_specular.r + average_specular.g + average_specular.b;
    float diffuse_probability = diffuse_power / (diffuse_power + specular_power + 0.000001f);
    //std::cout << "Selected prob.: " << diffuse_probability << std::endl;
    mixbxdf->amt1 = diffuse_probability;
    bxdf = std::move(mixbxdf);
    */
    auto ltc_diffused_bxdf = std::make_unique<BxDFLTCDiffuse<LTC::GGX>>();
    ltc_diffused_bxdf->roughness = roughness;
    ltc_diffused_bxdf->diffuse = diffuse;
    ltc_diffused_bxdf->color = specular;
    bxdf = std::move(ltc_diffused_bxdf);

    out::cout(4) << "Read material: " << name << std::endl;

}

// ================ BXDF ===============



// ================ Diffuse ===============

Spectrum BxDFDiffuse::value(glm::vec3 Vi, glm::vec3 Vr, glm::vec2 texUV, bool) const{
    if(Vi.z <= 0 || Vr.z <= 0) return Spectrum(0);
    return diffuse->GetSpectrum(texUV) / glm::pi<float>();
}

std::tuple<glm::vec3, Spectrum, bool>
BxDFDiffuse::sample(glm::vec3 Vi, glm::vec2 texUV, glm::vec2 sample, bool) const{
    if(Vi.z <= 0) return std::make_tuple(glm::vec3(0,1,0), Spectrum(0), false);

    glm::vec3 v = RandomUtils::Sample2DToHemisphereCosineZ(sample);
    qassert_true(v.z >= -0.001f);
    return std::make_tuple(v,diffuse->GetSpectrum(texUV), false);
}


void BxDFDiffuse::LoadFromJson(Json::Value& node, Scene& scene, std::string texturedir) {
    std::string texfile;
    texfile = JsonUtils::getOptionalString(node,"diffuse-texture","");
    if(texfile != "")
        diffuse = scene.GetTexture(texturedir + "/" + texfile);
    else if(node.isMember("diffuse") || node.isMember("diffuse255"))
        diffuse = scene.CreateSolidTexture(JsonUtils::getRequiredVec3_255(node, "diffuse"));
    else
        diffuse = scene.CreateSolidTexture(Color(0.5, 0.5, 0.5));
}


// ================ Mix ===============

void BxDFMix::LoadFromJson(Json::Value& node, Scene& scene, std::string) {
    std::string mat1 = JsonUtils::getRequiredString(node, "material1");
    std::string mat2 = JsonUtils::getRequiredString(node, "material2");
    try{ m1 = scene.GetMaterialByName(mat1);
    }catch(const std::exception& e){
        throw ConfigFileException("Material \"" + mat1 + "\", used for mixing, was not (yet) defined");
    }
    try{ m2 = scene.GetMaterialByName(mat2);
    }catch(const std::exception& e){
        throw ConfigFileException("Material \"" + mat2 + "\", used for mixing, was not (yet) defined");
    }
    amt1 = JsonUtils::getRequiredFloat(node, "amount");
}

Spectrum BxDFMix::value(glm::vec3 Vi, glm::vec3 Vr, glm::vec2 texUV, bool debug) const{
    Spectrum s1 = m1->bxdf->value(Vi,Vr,texUV,debug);
    Spectrum s2 = m2->bxdf->value(Vi,Vr,texUV,debug);
    return s1*amt1 + s2*(1.0f-amt1);
}

std::tuple<glm::vec3, Spectrum, bool>
BxDFMix::sample(glm::vec3 Vi, glm::vec2 texUV, glm::vec2 sample, bool debug) const{
    if(RandomUtils::DecideAndRescale(sample.x, amt1)){
        auto p = m1->bxdf->sample(Vi, texUV, sample, debug);
        return p;
    }else{
        return m2->bxdf->sample(Vi, texUV, sample, debug);
    }
}


// ================ Mirror ===============

void BxDFMirror::LoadFromJson(Json::Value& node, Scene& scene, std::string texturedir) {
    std::string texfile;
    texfile = JsonUtils::getOptionalString(node,"color-texture","");
    if(texfile != "")
        color = scene.GetTexture(texturedir + "/" + texfile);
    else if(node.isMember("color") || node.isMember("color255"))
        color = scene.CreateSolidTexture(JsonUtils::getRequiredVec3_255(node, "color"));
    else
        color = scene.CreateSolidTexture(Color(1.0, 1.0, 1.0));
}

Spectrum BxDFMirror::value(glm::vec3 Vi, glm::vec3 Vr, glm::vec2 texUV, bool) const{
    glm::vec3 reflected(-Vi.x, -Vi.y, Vi.z);
    if(glm::abs(glm::dot(reflected, Vr)-1) < 0.0001f){
        return color->GetSpectrum(texUV);
    }else return Spectrum(0.0f);
}

std::tuple<glm::vec3, Spectrum, bool>
BxDFMirror::sample(glm::vec3 Vi, glm::vec2 texUV, glm::vec2, bool) const{
    glm::vec3 reflected(-Vi.x, -Vi.y, Vi.z);
    return std::make_tuple(reflected, color->GetSpectrum(texUV), false);
}


// ================ LTC ===============

void BxDFLTCBase::LoadFromJson(Json::Value& node, Scene& scene, std::string texturedir){
    if(node.isMember("roughness"))
        roughness = JsonUtils::getRequiredFloat(node, "roughness");
    else if(node.isMember("exponent")){
        float phong_exp = JsonUtils::getRequiredFloat(node, "exponent");
        roughness = glm::pow(2.0f / (2.0f + phong_exp), 0.5f); ;
    }else{
        throw ConfigFileException("Either \"roughness\" or \"exponent\" must be present for LTC BxDF in " + JsonUtils::getNodeSemanticName(node));
    }

    std::string texfile;
    texfile = JsonUtils::getOptionalString(node,"color-texture",texfile = JsonUtils::getOptionalString(node,"specular-texture",""));
    if(texfile != "")
        color = scene.GetTexture(texturedir + "/" + texfile);
    else if(node.isMember("color") || node.isMember("color255"))
        color = scene.CreateSolidTexture(JsonUtils::getRequiredVec3_255(node, "color"));
    else if(node.isMember("specular") || node.isMember("specular255"))
        color = scene.CreateSolidTexture(JsonUtils::getRequiredVec3_255(node, "specular"));
    else
        color = scene.CreateSolidTexture(Color(0.0, 0.0, 0.0));
}

void BxDFLTCDiffuseBase::LoadFromJson(Json::Value& node, Scene& scene, std::string texturedir){

    BxDFLTCBase::LoadFromJson(node,scene,texturedir);

    std::string texfile;
    texfile = JsonUtils::getOptionalString(node,"diffuse-texture","");
    if(texfile != "")
        diffuse = scene.GetTexture(texturedir + "/" + texfile);
    else if(node.isMember("diffuse") || node.isMember("diffuse255"))
        diffuse = scene.CreateSolidTexture(JsonUtils::getRequiredVec3_255(node, "diffuse"));
    else
        diffuse = scene.CreateSolidTexture(Color(0,0,0));
}

// ================ Dielectric ===============

void BxDFDielectric::LoadFromJson(Json::Value& node, Scene& scene, std::string texturedir) {
    ior = JsonUtils::getRequiredFloat(node, "ior");

    std::string texfile;
    texfile = JsonUtils::getOptionalString(node,"color-texture",texfile = JsonUtils::getOptionalString(node,"specular-texture",""));
    if(texfile != "")
        color = scene.GetTexture(texturedir + "/" + texfile);
    else if(node.isMember("color") || node.isMember("color255"))
        color = scene.CreateSolidTexture(JsonUtils::getRequiredVec3_255(node, "color"));
    else
        color = scene.CreateSolidTexture(Color(1.0, 1.0, 1.0));
}

static std::pair<float, float>
FresnellDielectric(float eta, float cosTheta, bool debug = false){

    if (cosTheta < 0.0f) {
        eta = 1.0f/eta;
        cosTheta = -cosTheta;
    }
    float sinThetaTSq = eta*eta*(1.0f - cosTheta*cosTheta);
    if (sinThetaTSq > 1.0f) {
        return {1.0, 0.0};
    }
    float cosThetaTrans = glm::sqrt(glm::max(1.0f - sinThetaTSq, 0.0f));

    IFDEBUG std::cout << "[BxDF] ThetaInc: " << glm::acos(cosTheta)*180.0f/glm::pi<float>() << std::endl;
    IFDEBUG std::cout << "[BxDF] ThetaTrans: " << glm::acos(cosThetaTrans)*180.0f/glm::pi<float>() << std::endl;

    float Rs = (eta*cosTheta      - cosThetaTrans)/(eta*cosTheta      + cosThetaTrans);
    float Rp = (eta*cosThetaTrans - cosTheta     )/(eta*cosThetaTrans + cosTheta     );
    IFDEBUG std::cout << "[BxDF] Rs: " << Rs*Rs << std::endl;
    IFDEBUG std::cout << "[BxDF] Rp: " << Rp*Rp << std::endl;
    float R = 0.5f*(Rs*Rs + Rp*Rp);
    return {R, cosThetaTrans};
}

Spectrum BxDFDielectric::value(glm::vec3 Vi, glm::vec3 Vr, glm::vec2 texUV, bool) const{

    float eta;
    if(Vi.z < 0) eta = ior;
    else eta = 1.0/ior;

    float reflectionP, cosTheta;
    std::tie(reflectionP, cosTheta) = FresnellDielectric(eta, Vi.z);

    Spectrum c = color->GetSpectrum(texUV);

    if(Vi.z * Vr.z > 0){
        // Same side
        glm::vec3 reflected(-Vi.x, -Vi.y, Vi.z); // This way, rays reflected from below surface stay below surface
        if(glm::abs(glm::dot(Vr, reflected) - 1) < 0.001f) return Spectrum(reflectionP) * c;
        else return Spectrum(0.0f);
    }else{
        // Opposite sides
        glm::vec3 refracted(-Vi.x * eta, -Vi.y * eta, (Vi.z > 0) ? -cosTheta : cosTheta);
        if(glm::abs(glm::dot(Vr, refracted) - 1) < 0.001f) return Spectrum(1.0f - reflectionP) * c;
        else return Spectrum(0.0f);
    }
}

std::tuple<glm::vec3, Spectrum, bool>
BxDFDielectric::sample(glm::vec3 Vi, glm::vec2 texUV, glm::vec2 sample, bool debug) const{
    // Note: This is, naturally a bidirectional material.
    float eta;
    if(Vi.z < 0) eta = ior;
    else eta = 1.0/ior;

    float reflectionP, cosTheta;
    std::tie(reflectionP, cosTheta) = FresnellDielectric(eta, std::abs(Vi.z), debug);

    Spectrum c = color->GetSpectrum(texUV);
    IFDEBUG std::cout << "[BxDF] Eta: " << eta << std::endl;
    IFDEBUG std::cout << "[BxDF] CosThetaInc: " << Vi.z << std::endl;
    IFDEBUG std::cout << "[BxDF] Reflection probability: " << reflectionP << std::endl;
    IFDEBUG std::cout << "[BxDF] CosThetaRefr: " << cosTheta << std::endl;

    if(RandomUtils::DecideAndRescale(sample.x, reflectionP)){
        // Reflected ray
        IFDEBUG std::cout << "[BxDF] Ray reflected" << std::endl;
        glm::vec3 reflected(-Vi.x, -Vi.y, Vi.z); // This way, rays reflected from below surface stay below surface
        return std::make_tuple(reflected, c, false);
    }else{
        // Refracted ray
        IFDEBUG std::cout << "[BxDF] Ray refracted" << std::endl;
        cosTheta = std::abs(cosTheta);
        glm::vec3 refracted(-Vi.x * eta, -Vi.y * eta, (Vi.z > 0) ? -cosTheta : cosTheta);
        return std::make_tuple(refracted, c, true);
    }
}

// ================ Transparent ===============

Spectrum BxDFTransparent::value(glm::vec3 Vi, glm::vec3 Vr, glm::vec2, bool) const{
    glm::vec3 inverse(-Vi.x, -Vi.y, -Vi.z);
    if(glm::abs(glm::dot(inverse, Vr)-1) < 0.0001f){
        return Spectrum(1.0f);
    }else return Spectrum(0.0f);
}

std::tuple<glm::vec3, Spectrum, bool>
BxDFTransparent::sample(glm::vec3 Vi, glm::vec2, glm::vec2, bool) const{
    glm::vec3 inverse(-Vi.x, -Vi.y, -Vi.z);
    return std::make_tuple(inverse, Spectrum(1.0f), true);
}
