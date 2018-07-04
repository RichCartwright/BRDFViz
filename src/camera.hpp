#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "glm.hpp"
#include "ray.hpp"

class Camera{
public:
    Camera(); 
    Camera(glm::vec3 pos, glm::vec3 la, glm::vec3 up, float yview, float xview, int xsize, int ysize, float focus_plane = 1.0f, float lens_size = 0.0f);

    /* xres, yres - targetted output image resolution
       x, y - pixel coordinates
       subres - grid resolution for dividning the (x,y) pixel into subpixels
       subx, suby - the coordinates of subpixel within (x,y) pixel for requesting ray
     */

    Ray GetRay(glm::vec2 coords) const;

    Ray GetPixelRay(int x, int y, int xres, int yres, glm::vec2 subcoords, glm::vec3* outpos = NULL) const;
    Ray GetPixelRayLens(int x, int y, int xres, int yres, glm::vec2 subcoords, glm::vec2 lenssample) const;

    bool IsSimple() const {return lens_size == 0.0f;}

    // Returns false if direction is not within camera view
    bool GetCoordsFromDirection(glm::vec3 dir, int& /*out*/ x, int& /*out*/ y, bool debug = false) const;
public:
    glm::vec3 origin;
    glm::vec3 lookat;
    glm::vec3 direction;

    glm::vec3 cameraup;
    glm::vec3 cameraleft;

    glm::vec3 viewscreen;
    glm::vec3 viewscreen_x;
    glm::vec3 viewscreen_y;

    float lens_size;

    int xsize;
    int ysize;

    glm::vec3 GetViewScreenPoint(float x, float y) const;

};

#endif // __CAMERA_HPP__
