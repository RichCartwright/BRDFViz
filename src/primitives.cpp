#include "primitives.hpp"
#include "scene.hpp"
#include "global_config.hpp"

#include <iomanip>

const Material& Triangle::GetMaterial() const {
    qassert_true(mat != nullptr);
    return *mat; }
const glm::vec3 Triangle::GetVertexA()  const { return parent_scene->vertices[va];  }
const glm::vec3 Triangle::GetVertexB()  const { return parent_scene->vertices[vb];  }
const glm::vec3 Triangle::GetVertexC()  const { return parent_scene->vertices[vc];  }
const glm::vec3 Triangle::GetNormalA()  const { return parent_scene->normals[va];  }
const glm::vec3 Triangle::GetNormalB()  const { return parent_scene->normals[vb];  }
const glm::vec3 Triangle::GetNormalC()  const { return parent_scene->normals[vc];  }
const glm::vec3 Triangle::GetTangentA()  const { return parent_scene->tangents[va];  }
const glm::vec3 Triangle::GetTangentB()  const { return parent_scene->tangents[vb];  }
const glm::vec3 Triangle::GetTangentC()  const { return parent_scene->tangents[vc];  }

const glm::vec2 Triangle::GetTexCoordsA()  const { return (parent_scene->n_texcoords <= va) ? glm::vec2(0.0,0.0) : parent_scene->texcoords[va]; }
const glm::vec2 Triangle::GetTexCoordsB()  const { return (parent_scene->n_texcoords <= va) ? glm::vec2(0.0,0.0) : parent_scene->texcoords[vb]; }
const glm::vec2 Triangle::GetTexCoordsC()  const { return (parent_scene->n_texcoords <= va) ? glm::vec2(0.0,0.0) : parent_scene->texcoords[vc]; }

void Triangle::CalculatePlane(){
    glm::vec3 v0 = parent_scene->vertices[va];
    glm::vec3 v1 = parent_scene->vertices[vb];
    glm::vec3 v2 = parent_scene->vertices[vc];

    glm::vec3 d0 = v1 - v0;
    glm::vec3 d1 = v2 - v0;

    glm::vec3 n = glm::normalize(glm::cross(d1,d0));
    float d = - glm::dot(n,v0);

    p = glm::vec4(n.x, n.y, n.z, d);
}

float Triangle::GetArea() const{
    glm::vec3 a = parent_scene->vertices[va];
    glm::vec3 b = parent_scene->vertices[vb];
    glm::vec3 c = parent_scene->vertices[vc];
    glm::vec3 q = a-b;
    glm::vec3 r = c-b;
    return 0.5f * glm::length(glm::cross(q,r));
}

/*
// According to paper at http://www.cs.princeton.edu/~funk/tog02.pdf
// page 814, this should give uniform resolution
glm::vec3 Triangle::GetRandomPoint(glm::vec2 sample) const{
    glm::vec2 r = sample;
    float sr1 = glm::sqrt(r.x);
    glm::vec3 a = parent_scene->vertices[va];
    glm::vec3 b = parent_scene->vertices[vb];
    glm::vec3 c = parent_scene->vertices[vc];
    return (1.0f-sr1)*a + sr1*(1.0f-r.y)*b + sr1*r.y*c;
}
*/

// Explaned at http://mathworld.wolfram.com/TrianglePointPicking.html
glm::vec3 Triangle::GetRandomPoint(glm::vec2 sample) const{
    glm::vec2 r = sample;
    glm::vec3 a = parent_scene->vertices[va];
    glm::vec3 c = parent_scene->vertices[vb];
    glm::vec3 b = parent_scene->vertices[vc];
    glm::vec3 Va = a-c;
    glm::vec3 Vb = b-c;
    if(r.x + r.y > 1.0f){
        r.x = 1.0f - r.x;
        r.y = 1.0f - r.y;
    }
    return c + r.x*Va + r.y*Vb;
}

bool Triangle::TestIntersection(const Ray& __restrict__ r, /*out*/ float& t, float& a, float& b, bool debug) const{
    // Currently using Badoulel's algorithm
    (void)debug;

    // This implementation is heavily inspired by the example provided by ANL

    const float eps = parent_scene->epsilon;

    //glm::vec3 planeN = p.xyz();
    glm::vec3 planeN = glm::vec3{p}; 
    double dot = glm::dot(r.direction, planeN);

    //IFDEBUG std::cout << "triangle " << GetVertexA() << " " << GetVertexB() << " " << GetVertexC() << " " << std::endl;
    //IFDEBUG std::cout << "dot : " << dot << std::endl;

    // TODO: Is it possible to elliminate such triangles during preprocessing?
    /* If the triangle is just a line segment, then it's normal will be NAN, and so dot will be nan. Ignore such triangles. */
    if(std::isnan(dot)) return false;

    /* is ray parallel to plane? */
    if (dot < eps && dot > -eps)
        return false;

    /* find distance to plane and intersection point */
    double dot2 = glm::dot(r.origin,planeN);
    t = -(p.w + dot2) / dot;

    // TODO: Accelerate this
    /* find largest dim*/
    int i1, i2;
    glm::vec3 pq = glm::abs(planeN);
    if(pq.x > pq.y && pq.x > pq.z){
        i1 = 1; i2 = 2;
    }else if(pq.y > pq.z){
        i1 = 0; i2 = 2;
    }else{
        i1 = 0; i2 = 1;
    }

    //IFDEBUG std::cout << i1 << "/" << i2 << " ";

    glm::vec3 vert0 = parent_scene->vertices[va];
    glm::vec3 vert1 = parent_scene->vertices[vb];
    glm::vec3 vert2 = parent_scene->vertices[vc];

    //IFDEBUG std::cerr << vert0 << " " << vert1 << " " << vert2 << std::endl;

    //IFDEBUG std::cerr << "intersection is at: " << r.origin + r.direction*t << std::endl;

    glm::vec2 point(r.origin[i1] + r.direction[i1] * t,
                    r.origin[i2] + r.direction[i2] * t);

    /* begin barycentric intersection algorithm */
    glm::vec2 q0( point[0] - vert0[i1],
                  point[1] - vert0[i2]);
    glm::vec2 q1( vert1[i1] - vert0[i1],
                  vert1[i2] - vert0[i2]);
    glm::vec2 q2( vert2[i1] - vert0[i1],
                  vert2[i2] - vert0[i2]);

    //IFDEBUG std::cerr << q0 << " " << q1 << " " << q2 << std::endl;

    // TODO Return these
    float alpha, beta;

    /* calculate and compare barycentric coordinates */
    if (q1.x > -eps && q1.x < eps ) {		/* uncommon case */
        //IFDEBUG std::cout << "UNCOMMON" << std::endl;
        beta = q0.x / q2.x;
        if (beta < 0 || beta > 1)
            return false;
        alpha = (q0.y - beta * q2.y) / q1.y;
    }
    else {			/* common case, used for this analysis */
        beta = (q0.y * q1.x - q0.x * q1.y) / (q2.y * q1.x - q2.x * q1.y);
        if (beta < 0 || beta > 1)
            return false;

        alpha = (q0.x - beta * q2.x) / q1.x;
    }

    //IFDEBUG std::cout << "A: " << alpha << ", B: " << beta << " A+B: " << alpha+beta << std::endl;

    if (alpha < 0 || (alpha + beta) > 1.0)
        return false;

    a = alpha;
    b = beta;

    return true;

}

primitive_data Primitives::planeY = {
    std::make_tuple(glm::vec3{ 1.0, 0.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0, 0.0, -1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0, 0.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0, 0.0, -1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{0.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0, 0.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0, 0.0, -1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
};

primitive_data Primitives::trigY = {
    std::make_tuple(glm::vec3{ 1.0, 0.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0, 0.0, -1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0, 0.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
};

primitive_data Primitives::cube = {
    // Far X wall
    std::make_tuple(glm::vec3{ 1.0,  1.0,  1.0}, glm::vec3{1.0, 0.0, 0.0}, glm::vec2{1.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0,  1.0, -1.0}, glm::vec3{1.0, 0.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0,  1.0}, glm::vec3{1.0, 0.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0, -1.0}, glm::vec3{1.0, 0.0, 0.0}, glm::vec2{0.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0,  1.0}, glm::vec3{1.0, 0.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{ 1.0,  1.0, -1.0}, glm::vec3{1.0, 0.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    // Near X wall
    std::make_tuple(glm::vec3{-1.0,  1.0,  1.0}, glm::vec3{-1.0, 0.0, 0.0}, glm::vec2{1.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0, -1.0}, glm::vec3{-1.0, 0.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0, -1.0,  1.0}, glm::vec3{-1.0, 0.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0, -1.0, -1.0}, glm::vec3{-1.0, 0.0, 0.0}, glm::vec2{0.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0, -1.0,  1.0}, glm::vec3{-1.0, 0.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 0.0, 1.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0, -1.0}, glm::vec3{-1.0, 0.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 0.0, 1.0}),

    // Far Y wall
    std::make_tuple(glm::vec3{ 1.0,  1.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 1.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0,  1.0, -1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0, -1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{0.0, 0.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0,  1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0,  1.0, -1.0}, glm::vec3{0.0, 1.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{1.0, 0.0, 0.0}),
    // Near Y wall
    std::make_tuple(glm::vec3{-1.0, -1.0,  1.0}, glm::vec3{0.0, -1.0, 0.0}, glm::vec2{1.0, 1.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0, -1.0, -1.0}, glm::vec3{0.0, -1.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0,  1.0}, glm::vec3{0.0, -1.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0, -1.0}, glm::vec3{0.0, -1.0, 0.0}, glm::vec2{0.0, 0.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0,  1.0}, glm::vec3{0.0, -1.0, 0.0}, glm::vec2{0.0, 1.0}, glm::vec3{1.0, 0.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0, -1.0, -1.0}, glm::vec3{0.0, -1.0, 0.0}, glm::vec2{1.0, 0.0}, glm::vec3{1.0, 0.0, 0.0}),

    // Far Z wall
    std::make_tuple(glm::vec3{ 1.0,  1.0,  1.0}, glm::vec3{0.0, 0.0, 1.0}, glm::vec2{1.0, 1.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0,  1.0}, glm::vec3{0.0, 0.0, 1.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0,  1.0}, glm::vec3{0.0, 0.0, 1.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0, -1.0,  1.0}, glm::vec3{0.0, 0.0, 1.0}, glm::vec2{0.0, 0.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0,  1.0}, glm::vec3{0.0, 0.0, 1.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0,  1.0}, glm::vec3{0.0, 0.0, 1.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 1.0, 0.0}),
    // Near Z wall
    std::make_tuple(glm::vec3{ 1.0,  1.0, -1.0}, glm::vec3{0.0, 0.0, -1.0}, glm::vec2{1.0, 1.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0, -1.0}, glm::vec3{0.0, 0.0, -1.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0, -1.0}, glm::vec3{0.0, 0.0, -1.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0, -1.0, -1.0}, glm::vec3{0.0, 0.0, -1.0}, glm::vec2{0.0, 0.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{ 1.0, -1.0, -1.0}, glm::vec3{0.0, 0.0, -1.0}, glm::vec2{0.0, 1.0}, glm::vec3{0.0, 1.0, 0.0}),
    std::make_tuple(glm::vec3{-1.0,  1.0, -1.0}, glm::vec3{0.0, 0.0, -1.0}, glm::vec2{1.0, 0.0}, glm::vec3{0.0, 1.0, 0.0}),
};
