#ifndef __OUT_HPP__
#define __OUT_HPP__

#include <iostream>

class out{
private:
    out(std::ostream& o, bool discard) : o(o), discard(discard) {}

    std::ostream& o;
    bool discard = false;
public:
    void flush(){ std::flush(o); }

    static out cout(unsigned int level){
        return out(std::cout, level > verbosity_level);
    }
    static out cerr(unsigned int level){
        return out(std::cerr, level > verbosity_level);
    }

    template<typename T>
    out& operator<<(const T& t){
        if(!discard) o << t;
        return *this;
    }

    out& operator<<(std::ostream& (*f)(std::ostream&)){
        if(!discard) f(o);
        return *this;
    }

    static unsigned int verbosity_level;
};




#endif // __OUT_HPP__
