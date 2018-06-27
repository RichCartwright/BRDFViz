#ifndef __SAMPLER_HPP__
#define __SAMPLER_HPP__

#include "glm.hpp"
#include "out.hpp"
#include <random>
#include <algorithm>

#include "HoradamGenerator.h"
//#include "../external/halton_sampler.h"

//#define M_PI   3.1415926535897932384626433832795

class Sampler{
public:
    Sampler(unsigned int seed) : seed(seed) {}
    virtual void Advance() = 0;
    virtual float Get1D() = 0;
    virtual glm::vec2 Get2D() = 0;
    virtual std::pair<unsigned int, unsigned int> GetUsage() const = 0;
protected:
    unsigned int seed;
};

class IndependentSampler : public Sampler{
public:
    IndependentSampler(unsigned int seed) : Sampler(seed), gen(seed){
    }
    virtual void Advance() override {};
    virtual float Get1D() override{
        return std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
    }
    virtual glm::vec2 Get2D() override{
        float x = std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
        float y = std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
        return {x,y};
    }
    virtual std::pair<unsigned int, unsigned int> GetUsage() const override{
        return {0,0};
    }
protected:
    std::mt19937 gen;
};

class OfflineSampler : public Sampler{
public:
    OfflineSampler(unsigned int seed, unsigned int dim, unsigned int set_size);
    virtual void PrepareSamples() = 0;
    virtual void Advance() override;
    virtual float Get1D() override;
    virtual glm::vec2 Get2D() override;
    virtual std::pair<unsigned int, unsigned int> GetUsage() const override{
        return {current_sample1D,current_sample2D};
    }
protected:
    std::vector<std::vector<float>> samples1D;
    std::vector<std::vector<glm::vec2>> samples2D;
    const unsigned int dim_count;
    const unsigned int set_size;
    unsigned int current_sample1D;
    unsigned int current_sample2D;
    unsigned int current_set;
    std::mt19937 gen;
};

class LatinHypercubeSampler : public OfflineSampler{
public:
    LatinHypercubeSampler(unsigned int seed, unsigned int dim, unsigned int set_size)
        : OfflineSampler(seed, dim, set_size){
    }
    virtual void PrepareSamples() override;
};

class IndependentOfflineSampler : public OfflineSampler{
public:
    IndependentOfflineSampler(unsigned int seed, unsigned int dim, unsigned int set_size)
        : OfflineSampler(seed, dim, set_size){
    }
    virtual void PrepareSamples() override;
};

class StratifiedSampler : public OfflineSampler{
public:
    StratifiedSampler(unsigned int seed, unsigned int dim, unsigned int ssize);
    virtual void PrepareSamples() override;
};

class VanDerCoruptSampler : public OfflineSampler{
public:
    VanDerCoruptSampler(unsigned int seed, unsigned int dim, unsigned int set_size)
        : OfflineSampler(seed, dim, set_size){
    }
    virtual void PrepareSamples() override;
};


// Halton sequence

template<unsigned int base>
class Halton {
public:
    Halton(){
		inv_base = 1.0f/base;
        value = 0.0f;
    }
	Halton(int i) {
		float f = inv_base = 1.0f/base;
		value = 0.0f;
		while(i>0) {
			value += f*(double)(i%base);
			i /= base;
			f *= inv_base;
		}
	}
	float next() {
        float v = value;
		float r = 1.0 - value - 0.0000001;
		if(inv_base<r) value += inv_base;
		else {
			double h = inv_base, hh;
			do {hh = h; h *= inv_base;} while(h >=r);
			value += hh + h - 1.0;
		}
        return v;
	}
private:
	float value;
    float inv_base;
};
/*
class HaltonSampler : public OfflineSampler{
public:
    HaltonSampler(unsigned int seed, unsigned int dim, unsigned int set_size)
        : OfflineSampler(seed, dim, set_size){
        //hs.init_faure();
        std::minstd_rand0 g(seed);
        // Silly HS::Halton_sampler does not accept rvalue references

        // TODO: This init works far too slow.
        hs.init_random(g);
    }
    virtual void PrepareSamples() override{
        for(unsigned int dim = 0; dim < dim_count; dim++){
            for(unsigned int sample = 0; sample < set_size; sample++){
                samples1D[dim][sample] = hs.sample(dim,sample);
            }
            std::shuffle(samples1D[dim].begin(), samples1D[dim].end(), gen);

            for(unsigned int sample = 0; sample < set_size; sample++){
                samples2D[dim][sample] = {hs.sample(dim,sample*2), hs.sample(dim,sample*2+1)};
            }
            std::shuffle(samples2D[dim].begin(), samples2D[dim].end(), gen);
        }
    }
private:
    HS::Halton_sampler hs;
};
*/
class HoradamSampler : public IndependentSampler 
{
public:
    	HoradamSampler(unsigned int seed) : IndependentSampler(seed) 
	{
		float x1 = sqrtf(2.0f) / 3.0f;
    		float x2 = sqrtf(5.0f) / 15.0f;

		std::complex<float> z1 = std::exp(std::complex<float>(0.0f, 2.0f*M_PI*x1));
		std::complex<float> z2 = std::exp(std::complex<float>(0.0f, 2.0f*M_PI*x2));

		horadam.InitHoradamGenerator(std::complex<float>(2.0f, 2.0f/3.0f), std::complex<float>(3.0f, 1.0f), z1+z2, z1*z2);

		bound = horadam.GetBound();
		norm = 1.0f/(2.0f*bound);
	}
   	int _x = std::uniform_int_distribution<>(1, 2)(gen); 
	int _y = std::uniform_int_distribution<>(3, 4)(gen);

	virtual void Advance() override {};
	virtual float Get1D() override
	{
		std::complex<float> next = horadam.GetNext(1);
		return (next.real() + bound)*norm;
	}
	virtual glm::vec2 Get2D() override
	{
		std::complex<float> next = horadam.GetNext(1);
		float x = (next.real()+bound)*norm;
		float y = (next.imag()+bound)*norm;

		return {x,y};
	}
	
	virtual std::pair<unsigned int, unsigned int> GetUsage() const override
    	{
		return {0,0};
    	}

private:
	HoradamGenerator<float> horadam;
	float bound;
	float norm;
};

#endif // __SAMPLER_HPP__
