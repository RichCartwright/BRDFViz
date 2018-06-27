#include "../global_config.hpp"
#include "../glm.hpp"

struct mat33
{
    operator glm::mat3() const
    {
        return glm::mat3(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
    }

    double m[9];
};

struct LTCdef{
    const int size;
    const mat33* tabM;
    const float* tabAmplitude;
};

class LTC{
public:
    // Returns PDF value of a LTC approximation for a given normal
    // vector, incoming/reflected vectors, and roughness.
    static float GetPDF(LTCdef ltc, glm::vec3 N, glm::vec3 Vr, glm::vec3 Vi, float alpha, bool debug = false);
    static float GetPDFZ(LTCdef ltc, glm::vec3 Vr, glm::vec3 Vi, float alpha, bool debug = false);
    // Given a random Z-oriented vector rand_hscos, this method
    // applies the linear transform that approximates the LTC BRDF.
    static glm::vec3 GetRandom(LTCdef ltc, glm::vec3 normal, glm::vec3 incoming, float roughness, glm::vec3 rand_hscos, bool debug = false);
    static glm::vec3 GetRandomZ(LTCdef ltc, glm::vec3 incoming, float roughness, glm::vec3 rand_hscos, bool debug = false);

    static const LTCdef Beckmann;
    static const LTCdef GGX;
private:
    static std::pair<glm::mat3, float> get_bilinear(LTCdef ltc, const float theta, const float alpha, bool debug = false);
};
