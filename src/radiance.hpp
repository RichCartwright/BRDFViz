#ifndef __RADIANCE_HPP__
#define __RADIANCE_HPP__

#include "glm.hpp"

struct Color{
    Color() : r(0.0), g(0.0), b(0.0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
    Color(const glm::vec3& c) : r(c.r), g(c.g), b(c.b) {}
    float r,g,b; // 0 - 1
    Color  operator* (float q)            const {return Color(q*r, q*g, q*b);}
    Color  operator/ (float q)            const {return Color(r/q, g/q, b/q);}
    Color  operator* (const Color& other) const {return Color(other.r*r, other.g*g, other.b*b);}
    Color  operator+ (const Color& o)     const {return Color(r+o.r,g+o.g,b+o.b);}
    Color& operator+=(const Color& o) {*this = *this + o; return *this;}
    Color gammaEncode(float gamma = 2.2f){
        return Color(glm::pow(r, 1.0f/gamma),
                     glm::pow(g, 1.0f/gamma),
                     glm::pow(b, 1.0f/gamma));
    }
    Color gammaDecode(float gamma = 2.2f){
        return Color(glm::pow(r, gamma),
                     glm::pow(g, gamma),
                     glm::pow(b, gamma));
    }
};

inline Color operator*(float q, const Color& c){
    return Color(q*c.r, q*c.g, q*c.b);
}

struct Radiance{
    Radiance() : r(0.0), g(0.0), b(0.0) {}
    Radiance(float r, float g, float b) : r(r), g(g), b(b) {}
    explicit Radiance(const Color& c){
        // Clearly this is a wrong place to apply gamma
        const float gamma = 1.0f;
        r = pow(c.r, gamma);
        g = pow(c.g, gamma);
        b = pow(c.b, gamma);
    }
    float r,g,b; // unbounded, positive
    Radiance  operator/ (float q)           const {return Radiance(r/q, g/q, b/q);}
    Radiance  operator+ (const Radiance& o) const {return Radiance(r+o.r,g+o.g,b+o.b);}
    Radiance  operator- (const Radiance& o) const {return Radiance(r-o.r,g-o.g,b-o.b);}
    Radiance& operator+=(const Radiance& o) {*this = *this + o; return *this;}
    Radiance& operator-=(const Radiance& o) {*this = *this - o; return *this;}
    float max() const {return glm::max(glm::max(r,g),b);}
    bool isNonZero() const {return r > 0 || g > 0 || b > 0;}
    void clamp(float v){
        if(r > v) r = v;
        if(g > v) g = v;
        if(b > v) b = v;
    }
};

struct Spectrum{
    Spectrum() : r(1.0), g(1.0), b(1.0) {}
    explicit Spectrum(float f) : r(f), g(f), b(f) {}
    Spectrum(float r, float g, float b) : r(r), g(g), b(b) {}
    explicit Spectrum(const Color& c){
        r = c.r; g = c.g, b = c.b;
    }
    Spectrum operator* (float q) const {return Spectrum(q*r, q*g, q*b);}
    Spectrum operator/ (float q) const {return Spectrum(r/q, g/q, b/q);}
    Spectrum operator* (const Spectrum& o) const {return Spectrum(o.r*r, o.g*g, o.b*b);}
    Spectrum operator*=(float q) {*this = *this * q; return *this;}
    Spectrum operator/=(float q) {*this = *this / q; return *this;}
    Spectrum operator*=(const Spectrum& o) {*this = *this * o; return *this;}

    Spectrum  operator+ (const Spectrum& o) const {return Spectrum(r+o.r,g+o.g,b+o.b);}
    Spectrum  operator- (const Spectrum& o) const {return Spectrum(r-o.r,g-o.g,b-o.b);}

    float max() const {return glm::max(glm::max(r,g),b);}

    float r,g,b;
};

inline Radiance operator*(const Radiance& r, const Spectrum& s){
    return Radiance(r.r * s.r, r.g * s.g, r.b * s.b);
}
inline Radiance operator*(const Spectrum& s, const Radiance& r){
    return Radiance(r.r * s.r, r.g * s.g, r.b * s.b);
}
inline Radiance& operator *=(Radiance& r, const Spectrum& s){
    r.r *= s.r; r.g *= s.g; r.b *= s.b;
    return r;
}



#endif //__RADIANCE_HPP__
