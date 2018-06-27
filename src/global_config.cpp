#define NO_EXTERN
#include "global_config.hpp"
#undef NO_EXTERN

#if ENABLE_DEBUG
bool debug_trace = false;
unsigned int debug_x, debug_y;
#endif

#include <signal.h>
#include <iostream>

void assert_fail(std::string text,std::string file,int line,std::string function){
    std::cerr << "Assertion failed." << std::endl;
    std::cerr << file << ":" << line << ", in " << function << std::endl;
    std::cerr << "because " << text << std::endl;
    raise(SIGABRT);
}
