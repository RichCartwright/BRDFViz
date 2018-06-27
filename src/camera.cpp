#include "camera.hpp"

#include "utils.hpp"
#include "glm.hpp"
#include "random_utils.hpp"

Camera::Camera(glm::vec3 pos, glm::vec3 la, glm::vec3 up, float yview, float xview, int xres, int yres, float focus_plane, float ls){
    origin = pos;
    lookat = la;
    cameraup = up;

    xsize = xres;
    ysize = yres;

    lens_size = ls;

    direction = glm::normalize( lookat - origin);
    cameraleft = glm::normalize(glm::cross(cameraup, direction));
    cameraup = glm::normalize(glm::cross(cameraleft, direction));

    viewscreen_x = - xview * cameraleft * focus_plane;
    viewscreen_y =   yview * cameraup * focus_plane;
    viewscreen = origin + direction * focus_plane - 0.5f * viewscreen_y - 0.5f * viewscreen_x;
}

glm::vec3 Camera::GetViewScreenPoint(float x, float y) const {
    glm::vec3 xo = x * viewscreen_x;
    glm::vec3 yo = y * viewscreen_y;
    return viewscreen + xo + yo;
}

Ray Camera::GetPixelRay(int x, int y, int xres, int yres, glm::vec2 subcoords, glm::vec3* outpos) const {
    glm::vec2 off = subcoords;
    glm::vec3 p = GetViewScreenPoint( (x + off.x) / (float)(xres),
                                      (y + off.y) / (float)(yres) );

    glm::vec3 o = origin;

    if(outpos != NULL) { 
	    *outpos = p ; 
    }

    return Ray(o, p - o);
}
Ray Camera::GetPixelRayLens(int x, int y, int xres, int yres, glm::vec2 subcoords, glm::vec2 lenssample) const{
    glm::vec2 off = subcoords;
    glm::vec3 p = GetViewScreenPoint( (x + off.x) / (float)(xres),
                                      (y + off.y) / (float)(yres) );
    glm::vec2 lenso = RandomUtils::Sample2DToDiscUniform(lenssample) * lens_size;
    glm::vec3 o = origin + lenso.x * cameraleft + lenso.y * cameraup;
    return Ray(o, p - o);
}

bool Camera::GetCoordsFromDirection(glm::vec3 dir, int& /*out*/ x, int& /*out*/ y, bool debug) const{
    (void)debug;
    // TODO: Rewrite this entirely.

    glm::vec3 N = direction;
    float q = glm::dot(dir, N);
    if(q < 0.0001) return false; // parallel
    float t = glm::dot(viewscreen - origin,N) / q;
    if(t <= 0) return false; // oriented outside camera
    //IFDEBUG std::cout << "t: " << t << std::endl;
    glm::vec3 p = origin + dir * t;

    glm::vec3 V = viewscreen;
    glm::vec3 v1 = viewscreen_x;
    glm::vec3 v2 = viewscreen_y;

    glm::vec3 vp = p - V;
    //IFDEBUG std::cout << "vp: " << vp << std::endl;
    float plen = glm::length(vp);
    // Cast vp onto v1 and v2
    float v1_cast_len = plen * (glm::dot(glm::normalize(vp), glm::normalize(v1)));
    float v2_cast_len = plen * (glm::dot(glm::normalize(vp), glm::normalize(v2)));
    //IFDEBUG std::cout << "lens: " << v1_cast_len <<  " " << v2_cast_len << std::endl;
    float x_ratio = v1_cast_len / glm::length(v1);
    float y_ratio = v2_cast_len / glm::length(v2);

    //IFDEBUG std::cout << "ratios: " << x_ratio <<  " " << y_ratio << std::endl;

    if(x_ratio < 0.0f || x_ratio > 1.0f || y_ratio < 0.0f || y_ratio > 1.0f) return false;

    x = xsize * x_ratio;
    y = ysize * y_ratio;

    return true;

}
