
#ifdef NDEBUG
#define ENABLE_DEBUG 0
#else
#define ENABLE_DEBUG 1
#endif

#define TILE_SIZE 32

#define PREVIEW_DIMENTIONS_RATIO 4
#define PREVIEW_RAYS_RATIO 2
#define PREVIEW_SPEED_RATIO (PREVIEW_DIMENTIONS_RATIO*PREVIEW_DIMENTIONS_RATIO*PREVIEW_RAYS_RATIO)

#define BARSIZE 75

// =======================================

#ifdef NDEBUG
  #undef ENABLE_DEBUG
  #define ENABLE_DEBUG 0
#endif

#if ENABLE_DEBUG
  #include <cassert>
  #define IFDEBUG if(debug)
#else
  #define IFDEBUG if(0)
  #undef assert
  #define assert(x) (void)0
#endif

#if ENABLE_DEBUG
  #ifndef NO_EXTERN
    extern bool debug_trace;
    extern unsigned int debug_x, debug_y;
  #endif // NO_EXTERN
#else
  #ifndef NDEBUG
    #define NDEBUG
  #endif
#endif

// ===== Extra stuff

#include <string>

#define NEAR(x,y) (x < y + 0.001f && x > y - 0.001f)


#if ENABLE_DEBUG

void assert_fail(std::string text,std::string file,int line,std::string function);
#define qassert_true(cond) ( (cond) ?           \
        static_cast<void>(0): \
                             assert_fail ("condition " + std::string(#cond) + " is not true" , __FILE__, __LINE__, __PRETTY_FUNCTION__) \
    )
#define qassert_false(cond) ( !(cond) ?         \
        static_cast<void>(0): \
                              assert_fail ("condition " + std::string(#cond) + " is not false" , __FILE__, __LINE__, __PRETTY_FUNCTION__) \
    )
#define qassert_less(a,b) ( a<b ?             \
        static_cast<void>(0): \
                            assert_fail ("value " + #a + " = " + a + " is not less than " + #b + " = " + b , __FILE__, __LINE__, __PRETTY_FUNCTION__) \
    )
#define qassert_directed(a,b) ( glm::dot(a,b)>0 ? \
        static_cast<void>(0): \
                                assert_fail (std::string("vector ") + #a + " = " + glm::to_string(a) + " is not in the direction of vector " + #b + " = " + glm::to_string(b) + ", dot is " + std::to_string(glm::dot(a,b))  , __FILE__, __LINE__, __PRETTY_FUNCTION__) \
    )

#else
  #define qassert_true(x) ((void)0)
  #define qassert_false(x) ((void)0)
  #define qassert_less(a,b) ((void)0)
  #define qassert_directed(a,b) ((void)0)
#endif
