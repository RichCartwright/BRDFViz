#include "utils.hpp"

#include <algorithm>
#include <fstream>
#include <locale>
#include <sstream>
#include <cmath>
#include <iomanip>

static inline std::string& ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

static inline std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

static inline std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

std::string Utils::Trim(std::string s){
  return trim(s);
}

std::vector<std::string> Utils::SplitString(std::string str, std::string delimiter, bool skip_empty){
    std::vector<std::string> res;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        if(! skip_empty || token != ""){
            res.push_back(token);
        }
        str.erase(0, pos + delimiter.length());
    }
    res.push_back(str);
    return res;
}

std::string Utils::JoinString(std::vector<std::string> str, std::string c){
	std::string buf = "";
	for(unsigned int i = 0; i < str.size(); i++){
		buf += str[i];
		if(i < str.size()-1) buf += c;
	}
	return buf;
}

std::string Utils::GetDir(std::string path){
  auto pos = path.rfind("/");
  if(pos == std::string::npos){
    return std::string(".") + "/";
  }else{
    return path.substr(0,pos);
  }
}
std::string Utils::GetFilename(std::string path){
  auto pos = path.rfind("/");
  if(pos == std::string::npos){
    return path;
  }else{
    return path.substr(pos+1);
  }
}

std::pair<std::string, std::string> Utils::GetFileExtension(std::string fname){
    auto v = SplitString(fname, ".");
    if(v.size() == 1) return {v[0], ""};
    std::string ext = v.back();
    v.pop_back();
    std::string name = JoinString(v, ".");
    return {name, ext};
}


std::string Utils::InsertFileSuffix(std::string path, std::string suffix){
    auto p = GetFileExtension(path);
    return p.first + "." + suffix + "." +  p.second;
}

bool Utils::GetFileExists(std::string name)
{
  // For compilers that support C++14 experimental TS:
  // std::experimental::filesystem::exists(name);
  // For C++17:
  // std::filesystem::exists(name);
  return (bool)std::ifstream(name);
}


std::string Utils::FormatIntThousands(unsigned int value){
    class comma_sep
        : public std::numpunct<char>
    {
        virtual char do_thousands_sep() const { return '\''; }
        virtual std::string do_grouping() const { return "\03"; }
    };

    std::stringstream ss;
    std::locale comma(std::locale(), new comma_sep());
    ss.imbue(comma);
    ss << value;
    return ss.str();
}


std::string Utils::FormatFraction5(float value){
    float i = 0;
    float f = std::modf(value, &i);
    if(f == 0.0f) return "00000";
    std::stringstream ss;
    ss << std::setprecision(5) << std::fixed << f;
    return SplitString(ss.str(), ".")[1];
}


std::string Utils::FormatInt5(unsigned int i){
    if(i == 0) return "00000";
    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << i;
    return ss.str();
}

std::string Utils::FormatTime(float s){
    int seconds = (s + 0.5f);
    int minutes = seconds / 60;
    int hours = minutes / 60;
    minutes %= 60;
    seconds %= 60;
    std::string result;
    if(hours > 0) result += std::to_string(hours) + "h ";
    if(hours > 0 || minutes > 0) result += std::to_string(minutes) + "m ";
    if(hours == 0) result += std::to_string(seconds) + "s ";
    if(result.back() == ' ')
        result.pop_back();
    return result;
}

std::string Utils::FormatPercent(float f){
    std::stringstream ss;
    ss << /*std::setw(5) <<*/ std::fixed << std::setprecision(1) << f << "%";
    return ss.str();
}


Utils::LowPass::LowPass(unsigned int size)
    : m_size(size){
}

float Utils::LowPass::Add(float value){
    if(std::isnan(value)) return value; // Do not store nans.
    data.push_back(value);
    while(data.size() > m_size) data.pop_front();
    // We might cache this value, but it would accumulate errors due to floating point numbers.
    // Adding ~50 numbers each 100 ms is not a big deal, though.
    float val = 0.0f;
    for(const float& f : data) val += f;
    return val / data.size(); // if size > 0, this will never divide by zero
}
