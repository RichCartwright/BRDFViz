#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <vector>
#include <list>
#include <iostream>

#include "glm.hpp"

#include "primitives.hpp"

inline std::ostream& operator<<(std::ostream& stream, const glm::vec3& v){
    stream << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const glm::mat3& v){
    stream << "[" << v[0][0] << ", " << v[0][1] << ", " << v[0][2] << "]" << std::endl;
    stream << "[" << v[1][0] << ", " << v[1][1] << ", " << v[1][2] << "]" << std::endl;
    stream << "[" << v[2][0] << ", " << v[2][1] << ", " << v[2][2] << "]" << std::endl;
    return stream;
}
inline std::ostream& operator<<(std::ostream& stream, const glm::vec2& v){
    stream << "[" << v.x << ", " << v.y << "]";
    return stream;
}
inline std::ostream& operator<<(std::ostream& stream, const Color& c){
    stream << "{" << c.r << ", " << c.g << ", " << c.b << "}";
    return stream;
}
inline std::ostream& operator<<(std::ostream& stream, const Radiance& c){
    stream << "{" << c.r << ", " << c.g << ", " << c.b << "}";
    return stream;
}
inline std::ostream& operator<<(std::ostream& stream, const Spectrum& c){
    stream << "{" << c.r << ", " << c.g << ", " << c.b << "}";
    return stream;
}

class Utils{
public:
    static std::string Trim(std::string);
    static std::vector<std::string> SplitString(std::string str, std::string delimiter, bool skip_empty = true);
    static std::string JoinString(std::vector<std::string> str, std::string c);
    static std::string FormatIntThousands(unsigned int value);
    static std::string FormatFraction5(float value);
    static std::string FormatInt5(unsigned int value);
    static std::string FormatTime(float seconds);
    static std::string FormatPercent(float f);
    static std::string GetDir(std::string path);
    // Extracts filename from a path
    static std::string GetFilename(std::string path);
    // Splits filename into name and extension
    static std::pair<std::string, std::string> GetFileExtension(std::string fname);
    static std::string InsertFileSuffix(std::string path, std::string suffix);
    static bool GetFileExists(std::string path);

    class LowPass{
    public:
        LowPass(unsigned int size);
        float Add(float value);
    private:
        unsigned int m_size;
        std::list<float> data;
    };
};

#endif // __UTILS_HPP__
