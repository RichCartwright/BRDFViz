#include "ltc.hpp"
#include "../utils.hpp"
#include <algorithm>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

// This function is not exactly pure, but we are guaranteed that the
// contents of whatever ltcdef points to are not changing.
inline glm::mat3 get_n(LTCdef ltc, int theta, int alpha) __attribute__((pure));
inline glm::mat3 get_n(LTCdef ltc, int theta, int alpha){
    return ltc.tabM[alpha + theta*ltc.size];
}
inline float getAMP_n(LTCdef ltc, int theta, int alpha) __attribute__((pure));
inline float getAMP_n(LTCdef ltc, int theta, int alpha){
    return ltc.tabAmplitude[alpha + theta*ltc.size];
}

std::pair<glm::mat3, float> LTC::get_bilinear(LTCdef ltc, const float theta, const float alpha, bool)
{
    float t = glm::max(0.0f, glm::min(1.0f, theta / (0.5f*3.14159f)));
    float a = glm::max(0.0f, glm::min(1.0f, sqrtf(alpha)));
    if(t >= 1.0f) t = 0.999f;
    if(a >= 1.0f) a = 0.999f;
    int s = ltc.size - 1;
    int t1 = floorf(t * s);
    int t2 = t1 + 1;
    int a1 = floorf(a * s);
    int a2 = a1 + 1;
    glm::mat3 Mt1a1 = get_n(ltc,t1,a1);
    glm::mat3 Mt1a2 = get_n(ltc,t1,a2);
    glm::mat3 Mt2a1 = get_n(ltc,t2,a1);
    glm::mat3 Mt2a2 = get_n(ltc,t2,a2);
    float At1a1 = getAMP_n(ltc,t1,a1);
    float At1a2 = getAMP_n(ltc,t1,a2);
    float At2a1 = getAMP_n(ltc,t2,a1);
    float At2a2 = getAMP_n(ltc,t2,a2);

    float dt1 = t*s - t1;
    float dt2 = t2 - t*s;
    float da1 = a*s - a1;
    float da2 = a2 - a*s;

    glm::mat3 resM =
        Mt1a1 * dt2 * da2 +
        Mt1a2 * dt2 * da1 +
        Mt2a1 * dt1 * da2 +
        Mt2a2 * dt1 * da1;
    float resAMP =
        At1a1 * dt2 * da2 +
        At1a2 * dt2 * da1 +
        At2a1 * dt1 * da2 +
        At2a2 * dt1 * da1;

	return {resM, resAMP};
}

float LTC::GetPDF(LTCdef ltc, glm::vec3 N, glm::vec3 Vr, glm::vec3 Vi, float alpha, bool debug){
    assert(alpha >= 0.0f && alpha <= 1.0f);
    (void)debug;

    glm::vec3 tangent = glm::cross(N,Vi);
    glm::vec3 Vi_cast = glm::cross(tangent,N);
    // X unit vector has to go to Vi_cast
    // Y unit vector has to go to tangent (direction does not matter)
    // Z unit vector has to go to N
    glm::mat3 rotate(Vi_cast, tangent, N);
    glm::mat3 unrotate = glm::inverse(rotate);

    glm::vec3 Vr3 = unrotate * Vr;

    float theta = glm::angle(Vi, N);
    auto q = get_bilinear(ltc, theta, alpha);
    glm::mat3 M = q.first;
    float amplitude = q.second;
    glm::mat3 invM = glm::inverse(M);
    glm::vec3 p = glm::normalize( invM * Vr3 );
    glm::vec3 Loriginal = p;
    glm::vec3 L_ = M * Loriginal;
    float l = glm::length(L_);
    float detM = glm::determinant(M);
	float Jacobian = detM / (l*l*l);
    float D = 1.0f / 3.14159f * glm::max<float>(0.0f, Loriginal.z);
    float res = amplitude * D / Jacobian;
    return res;
}


float LTC::GetPDFZ(LTCdef ltc, glm::vec3 Vr, glm::vec3 Vi, float alpha, bool debug){
    assert(alpha >= 0.0f && alpha <= 1.0f);
    (void)debug;

    glm::vec3 Vr3 = Vr;

    float theta = glm::angle(Vi, glm::vec3(0.0, 0.0, 1.0));

    auto q = get_bilinear(ltc, theta, alpha);
    glm::mat3 M = q.first;
    float amplitude = q.second;
    glm::mat3 invM = glm::inverse(M);
    glm::vec3 p = glm::normalize( invM * Vr3 );
    glm::vec3 Loriginal = p;
    glm::vec3 L_ = M * Loriginal;
    float l = glm::length(L_);
    float detM = glm::determinant(M);
	float Jacobian = detM / (l*l*l);
    float D = 1.0f / 3.14159f * glm::max<float>(0.0f, Loriginal.z);
    float res = amplitude * D / Jacobian;
    return res;
}

glm::vec3 LTC::GetRandom(LTCdef ltc, glm::vec3 N, glm::vec3 Vi, float roughness, glm::vec3 rand_hscos, bool debug){

    glm::vec3 tangent = glm::cross(N,Vi);
    glm::vec3 Vi_cast = glm::cross(tangent,N);
    // X unit vector has to go to Vi_cast
    // Y unit vector has to go to tangent (direction does not matter)
    // Z unit vector has to go to N
    glm::mat3 rotate(Vi_cast, tangent, N);

    float theta = glm::angle(Vi, N);
    auto q = get_bilinear(ltc, glm::max(theta, glm::pi<float>()/4.0f), roughness, debug);
    glm::mat3 M = q.first;

    IFDEBUG std::cout << "N = " << N << ", Vi = " << Vi << std::endl;
    IFDEBUG std::cout << "theta = " << theta << ", alpha = " << roughness << std::endl;
    IFDEBUG std::cout << "M = " << glm::to_string(M) << std::endl;

    qassert_false(std::isnan(rand_hscos.x));
    IFDEBUG std::cout << "rand_hscos = " << rand_hscos << std::endl;
    glm::vec3 s = M*rand_hscos;
    // This completly skews the distribution, but since we cannot
    // reject-sample the area that lays beyond the normal hemisphere,
    // some workaround is necesary
    if(s.z < 0.0001f) s.z = 0.0001f;

    qassert_directed(s, glm::vec3(0.0, 0.0, 1.0));

    IFDEBUG std::cout << "s1 = " << glm::normalize(s) << std::endl;
    s = rotate * s;
    return glm::normalize(s);
}


glm::vec3 LTC::GetRandomZ(LTCdef ltc, glm::vec3 Vi, float roughness, glm::vec3 rand_hscos, bool debug){
    assert(false);

    float theta = glm::angle(Vi, glm::vec3(0.0, 0.0, 1.0));
    auto q = get_bilinear(ltc, glm::max(theta, glm::pi<float>()/4.0f), roughness, debug);
    glm::mat3 M = q.first;

    IFDEBUG std::cout << "Vi = " << Vi << std::endl;
    IFDEBUG std::cout << "theta = " << theta << ", alpha = " << roughness << std::endl;
    IFDEBUG std::cout << "M = " << glm::to_string(M) << std::endl;

    qassert_false(std::isnan(rand_hscos.x));
    IFDEBUG std::cout << "rand_hscos = " << rand_hscos << std::endl;
    glm::vec3 s = M*rand_hscos;
    // This completly skews the distribution, but since we cannot
    // reject-sample the area that lays beyond the normal hemisphere,
    // some workaround is necesary
    // if(s.z < 0.0001f) s.z = 0.0001f;
    /*
    if(glm::dot(s, glm::vec3(0.0, 0.0, 1.0)) < 0){
        std::cout << "Vi = " << Vi << std::endl;
        std::cout << "theta = " << theta << ", alpha = " << roughness << std::endl;
        std::cout << "M = " << glm::to_string(M) << std::endl;
        std::cout << "rand_hscos = " << rand_hscos << std::endl;
    }

    qassert_directed(s, glm::vec3(0.0, 0.0, 1.0));
    */
    IFDEBUG std::cout << "s1 = " << glm::normalize(s) << std::endl;
    return glm::normalize(s);
}
