#include "sampler.hpp"
#include <algorithm>
#include <limits>

OfflineSampler::OfflineSampler(unsigned int seed, unsigned int dim, unsigned int set_size)
    : Sampler(seed),
      samples1D(dim, std::vector<float>(set_size)),
      samples2D(dim, std::vector<glm::vec2>(set_size)),
      dim_count(dim),
      set_size(set_size),
      gen(seed)
{
    current_sample1D = 0;
    current_sample2D = 0;
    // Advance MUST be called before accessing samples
    current_set = -1;
    // Cannot call PrepareSample from constructor as it is a virtual function!
}
void OfflineSampler::Advance(){
    if(current_set == (unsigned int)-1) PrepareSamples();
    current_sample1D = 0;
    current_sample2D = 0;
    current_set++;
    qassert_true(current_set < set_size);
}
float OfflineSampler::Get1D(){
    return (current_sample1D < dim_count) ?
        samples1D[current_sample1D++][current_set] :
        std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
}
glm::vec2 OfflineSampler::Get2D(){
    return (current_sample2D < dim_count) ?
        samples2D[current_sample2D++][current_set] :
        glm::vec2(std::uniform_real_distribution<float>(0.0f, 1.0f)(gen),
                  std::uniform_real_distribution<float>(0.0f, 1.0f)(gen));
}

void LatinHypercubeSampler::PrepareSamples(){
    for(unsigned int dim = 0; dim < dim_count; dim++){
        samples1D[dim] = std::vector<float>(set_size);
        samples2D[dim] = std::vector<glm::vec2>(set_size);

        // 1D LHS
        for(unsigned int sample = 0; sample < set_size; sample++){
            float begin = sample/(float)set_size;
            float len = 1.0f/(float)set_size;
            samples1D[dim][sample] = begin + std::uniform_real_distribution<float>(0.0f, len)(gen);
        }
        std::shuffle(samples1D[dim].begin(), samples1D[dim].end(), gen);

        // 2D LHS
        std::vector<unsigned int> LHS(set_size);
        for(unsigned int i = 0; i < set_size; i++) LHS[i] = i;
        std::shuffle(LHS.begin(), LHS.end(), gen);
        for(unsigned int sample = 0; sample < set_size; sample++){
            float xbegin = sample/(float)set_size;
            float ybegin = LHS[sample]/(float)set_size;
            float len = 1.0f/(float)set_size;
            float x = xbegin + std::uniform_real_distribution<float>(0.0f, len)(gen);
            float y = ybegin + std::uniform_real_distribution<float>(0.0f, len)(gen);
            samples2D[dim][sample] = glm::vec2(x,y);
        }
        std::shuffle(samples2D[dim].begin(), samples2D[dim].end(), gen);
    }
}

void IndependentOfflineSampler::PrepareSamples(){
    for(unsigned int dim = 0; dim < dim_count; dim++){
        for(unsigned int sample = 0; sample < set_size; sample++){
            samples1D[dim][sample] = std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
            samples2D[dim][sample] = glm::vec2(std::uniform_real_distribution<float>(0.0f, 1.0f)(gen),
                                               std::uniform_real_distribution<float>(0.0f, 1.0f)(gen));
        }
    }
}

static inline unsigned int round_up_to_square(unsigned int x){
    float s = std::sqrt(x);
    float i;
    float frac = std::modf(s, &i);
    if(frac < 0.0001f) return i*i;
    else return (i+1)*(i+1);
}

StratifiedSampler::StratifiedSampler(unsigned int seed, unsigned int dim, unsigned int ssize)
    : OfflineSampler(seed, dim, round_up_to_square(ssize)){
    if(ssize != set_size){
        //out::cout(6) << "Stratified sampler rounded set_size up from " << ssize << " to " << set_size << std::endl;
    }
}
void StratifiedSampler::PrepareSamples() {
    for(unsigned int dim = 0; dim < dim_count; dim++){
        for(unsigned int sample = 0; sample < set_size; sample++){
            float begin = sample/(float)set_size;
            float len = 1.0f/(float)set_size;
            samples1D[dim][sample] = begin + std::uniform_real_distribution<float>(0.0f, len)(gen);
        }
        std::shuffle(samples1D[dim].begin(), samples1D[dim].end(), gen);

        unsigned int sq = std::sqrt(set_size) + 0.5f;

        for(unsigned int sy = 0; sy < sq; sy++){
            for(unsigned int sx = 0; sx < sq; sx++){

                float len = 1.0f/(float)sq;
                float beginx = sx/(float)sq;
                float beginy = sy/(float)sq;
                float x = beginx + std::uniform_real_distribution<float>(0.0f, len)(gen);
                float y = beginy + std::uniform_real_distribution<float>(0.0f, len)(gen);

                samples2D[dim][sy*sq + sx] = glm::vec2(x,y);
            }
        }
        std::shuffle(samples2D[dim].begin(), samples2D[dim].end(), gen);
    }
}

float VanDerCorupt(uint32_t x){
    x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
    x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
    x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
    x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
    x = ((x >> 16) & 0xffffu) | ((x & 0xffffu) << 16);
    return (float)x/std::numeric_limits<uint32_t>::max();
}

void VanDerCoruptSampler::PrepareSamples() {
    for(unsigned int dim = 0; dim < dim_count; dim++){
        for(unsigned int sample = 0; sample < set_size; sample++){
            samples1D[dim][sample] = VanDerCorupt(sample);
        }
        std::shuffle(samples1D[dim].begin(), samples1D[dim].end(), gen);

        std::vector<float> tmp(set_size*2);
        for(unsigned int sample = 0; sample < set_size*2; sample++){
                tmp[sample] = VanDerCorupt(sample);
        }
        std::shuffle(tmp.begin(), tmp.end(), gen);
        for(unsigned int sample = 0; sample < set_size; sample++){
            samples2D[dim][sample] = {tmp[2*sample], tmp[2*sample + 1]};
        }
    }
}
