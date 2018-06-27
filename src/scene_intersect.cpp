#include "scene.hpp"
#include "bxdf/bxdf.hpp"

Intersection Scene::FindIntersectKd(const Ray& __restrict__ r) __restrict__ const{

    Intersection res;
    res.triangle = nullptr;
    res.t = std::numeric_limits<float>::infinity();


    // First, check whether the ray intersects with our BB at all.

    const  std::pair<float,float>* __restrict  bb[3] = {&xBB,&yBB,&zBB};

    // Inspired by pbrt's bbox intersection
    float t0 = r.near, t1 = r.far;
    for (int i = 0; i < 3; ++i) {
        float invRayDir = 1.f / r.direction[i];
        float tNear = ((*bb[i]).first  - r.origin[i]) * invRayDir;
        float tFar  = ((*bb[i]).second - r.origin[i]) * invRayDir;
        if (tNear > tFar) std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return res; // No intersection.
    }

    struct NodeToDo{
        const CompressedKdNode* node;
        float tmin, tmax;
    };


    glm::vec3 invDir(1.f/r.direction.x, 1.f/r.direction.y, 1.f/r.direction.z);

    NodeToDo todo[200];
    int todo_size = 1;
    todo[0] = NodeToDo{compressed_array, t0, t1};

    while(todo_size > 0){
        todo_size--;
        const CompressedKdNode* node = todo[todo_size].node;
        float tmin = todo[todo_size].tmin;
        float tmax = todo[todo_size].tmax;

        // Abort if we got too far.
        if(r.far < tmin) break;

        if(node->IsLeaf()){ // leaf node

            bool hit = false;
            // Search for intersections with triangles inside this node
            unsigned int n = node->GetTrianglesN();
            uint32_t tri_start = node->GetFirstTrianglePos();
            for(unsigned int p = 0; p < n; p++){
                // For each triangle ...
                unsigned int i = compressed_triangles[tri_start + p];
                const Triangle& tri = triangles[i];
                float t, a, b;
                //  ... test for an intersection
                if(tri.TestIntersection(r, t, a, b)){
                    if(t < tmin - epsilon || t > tmax + epsilon){
                        continue;
                    }
                    if(t < res.t){
                        // New closest intersect!
                        res.triangle = &tri;
                        res.t = t;

                        float c = 1.0f - a - b;
                        res.a = c;
                        res.b = a;
                        res.c = b;

                        hit = true;
                    }
                }
            }

            if(hit){
                return res;
            }

        }else{ // internal node

            // Compute parametric distance along ray to split plane
            int axis = node->GetSplitAxis();
            float tplane = (node->GetSplitPlane() - r.origin[axis]) * invDir[axis];

            // Get node children pointers for ray
            const CompressedKdNode *firstChild, *secondChild;
            int belowFirst = (r.origin[axis] <  node->GetSplitPlane()) ||
                             (r.origin[axis] == node->GetSplitPlane() && r.direction[axis] <= 0);
            if (belowFirst) {
                firstChild = node + 1;
                secondChild = compressed_array + node->GetOtherChildIndex();
            }else{
                firstChild = compressed_array + node->GetOtherChildIndex();
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tplane > tmax || tplane <= 0)
                todo[todo_size++] = NodeToDo{firstChild, tmin, tmax};
            else if (tplane < tmin)
                todo[todo_size++] = NodeToDo{secondChild, tmin, tmax};
            else {
                // Enqueue _secondChild_ in todo list
                todo[todo_size++] = NodeToDo{secondChild, tplane, tmax};
                todo[todo_size++] = NodeToDo{firstChild,  tmin, tplane};
            }
        }
    }

    // No hit found at all.
    return res;
}

const Triangle* Scene::FindIntersectKdAny(const Ray& __restrict__ r) __restrict__ const{

    // First, check whether the ray intersects with our BB at all.

    const  std::pair<float,float>* __restrict  bb[3] = {&xBB,&yBB,&zBB};

    // Inspired by pbrt's bbox intersection
    float t0 = r.near, t1 = r.far;
    for (int i = 0; i < 3; ++i) {
        float invRayDir = 1.f / r.direction[i];
        float tNear = ((*bb[i]).first  - r.origin[i]) * invRayDir;
        float tFar  = ((*bb[i]).second - r.origin[i]) * invRayDir;
        if (tNear > tFar) std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return nullptr; // No intersection.
    }

    struct NodeToDo{
        const CompressedKdNode* node;
        float tmin, tmax;
    };


    glm::vec3 invDir(1.f/r.direction.x, 1.f/r.direction.y, 1.f/r.direction.z);

    NodeToDo todo[200];
    int todo_size = 1;
    todo[0] = NodeToDo{compressed_array, t0, t1};

    while(todo_size > 0){
        todo_size--;
        const CompressedKdNode* node = todo[todo_size].node;
        float tmin = todo[todo_size].tmin;
        float tmax = todo[todo_size].tmax;

        // Abort if we got too far.
        if(r.far < tmin) break;

        if(node->IsLeaf()){ // leaf node
            // Search for intersections with triangles inside this node
            unsigned int n = node->GetTrianglesN();
            uint32_t tri_start = node->GetFirstTrianglePos();
            for(unsigned int p = 0; p < n; p++){
                // For each triangle ...
                unsigned int i = compressed_triangles[tri_start + p];
                const Triangle& tri = triangles[i];
                float t, a, b;
                //  ... test for an intersection
                if(tri.TestIntersection(r, t, a, b)){
                    if(t < tmin - epsilon || t > tmax + epsilon){
                        continue;
                    }
                    // Ignore whether this is the nearest intersection, we are looking for ANY.
                    return &tri;
                }
            }

        }else{ // internal node
            // Compute parametric distance along ray to split plane
            int axis = node->GetSplitAxis();
            float tplane = (node->GetSplitPlane() - r.origin[axis]) * invDir[axis];

            // Get node children pointers for ray
            const CompressedKdNode *firstChild, *secondChild;
            int belowFirst = (r.origin[axis] <  node->GetSplitPlane()) ||
                             (r.origin[axis] == node->GetSplitPlane() && r.direction[axis] <= 0);
            if (belowFirst) {
                firstChild = node + 1;
                secondChild = compressed_array + node->GetOtherChildIndex();
            }else{
                firstChild = compressed_array + node->GetOtherChildIndex();
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tplane > tmax || tplane <= 0)
                todo[todo_size++] = NodeToDo{firstChild, tmin, tmax};
            else if (tplane < tmin)
                todo[todo_size++] = NodeToDo{secondChild, tmin, tmax};
            else {
                // Enqueue _secondChild_ in todo list
                todo[todo_size++] = NodeToDo{secondChild, tplane, tmax};
                todo[todo_size++] = NodeToDo{firstChild,  tmin, tplane};
            }
        }
    }

    // No hit found at all.
    return nullptr;
}


Intersection Scene::FindIntersectKdOtherThan(const Ray& __restrict__ r, const Triangle* ignore) __restrict__ const{

    Intersection res;
    res.triangle = nullptr;
    res.t = std::numeric_limits<float>::infinity();


    // First, check whether the ray intersects with our BB at all.

    const  std::pair<float,float>* __restrict  bb[3] = {&xBB,&yBB,&zBB};

    // Inspired by pbrt's bbox intersection
    float t0 = r.near, t1 = r.far;
    for (int i = 0; i < 3; ++i) {
        float invRayDir = 1.f / r.direction[i];
        float tNear = ((*bb[i]).first  - r.origin[i]) * invRayDir;
        float tFar  = ((*bb[i]).second - r.origin[i]) * invRayDir;
        if (tNear > tFar) std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return res; // No intersection.
    }

    struct NodeToDo{
        const CompressedKdNode* node;
        float tmin, tmax;
    };


    glm::vec3 invDir(1.f/r.direction.x, 1.f/r.direction.y, 1.f/r.direction.z);

    NodeToDo todo[200];
    int todo_size = 1;
    todo[0] = NodeToDo{compressed_array, t0, t1};

    while(todo_size > 0){
        todo_size--;
        const CompressedKdNode* node = todo[todo_size].node;
        float tmin = todo[todo_size].tmin;
        float tmax = todo[todo_size].tmax;

        // Abort if we got too far.
        if(r.far < tmin) break;

        if(node->IsLeaf()){ // leaf node

            bool hit = false;
            // Search for intersections with triangles inside this node
            unsigned int n = node->GetTrianglesN();
            uint32_t tri_start = node->GetFirstTrianglePos();
            for(unsigned int p = 0; p < n; p++){
                // For each triangle ...
                unsigned int i = compressed_triangles[tri_start + p];
                const Triangle& tri = triangles[i];
                float t, a, b;

                // Skip the triangle if it matches ignore condition
                if(&tri == ignore) continue;

                //  ... test for an intersection
                if(tri.TestIntersection(r, t, a, b)){
                    if(t < tmin - epsilon || t > tmax + epsilon){
                        continue;
                    }
                    if(t < res.t){
                        // New closest intersect!
                        res.triangle = &tri;
                        res.t = t;

                        float c = 1.0f - a - b;
                        res.a = c;
                        res.b = a;
                        res.c = b;

                        hit = true;
                    }
                }
            }

            if(hit){
                return res;
            }

        }else{ // internal node

            // Compute parametric distance along ray to split plane
            int axis = node->GetSplitAxis();
            float tplane = (node->GetSplitPlane() - r.origin[axis]) * invDir[axis];

            // Get node children pointers for ray
            const CompressedKdNode *firstChild, *secondChild;
            int belowFirst = (r.origin[axis] <  node->GetSplitPlane()) ||
                             (r.origin[axis] == node->GetSplitPlane() && r.direction[axis] <= 0);
            if (belowFirst) {
                firstChild = node + 1;
                secondChild = compressed_array + node->GetOtherChildIndex();
            }else{
                firstChild = compressed_array + node->GetOtherChildIndex();
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tplane > tmax || tplane <= 0)
                todo[todo_size++] = NodeToDo{firstChild, tmin, tmax};
            else if (tplane < tmin)
                todo[todo_size++] = NodeToDo{secondChild, tmin, tmax};
            else {
                // Enqueue _secondChild_ in todo list
                todo[todo_size++] = NodeToDo{secondChild, tplane, tmax};
                todo[todo_size++] = NodeToDo{firstChild,  tmin, tplane};
            }
        }
    }

    // No hit found at all.
    return res;
}


Intersection Scene::FindIntersectKdOtherThanWithThinglass(const Ray& r, const Triangle* ignored) __restrict__ const{

    Intersection res;
    res.triangle = nullptr;
    res.t = std::numeric_limits<float>::infinity();


    // First, check whether the ray intersects with our BB at all.

    const  std::pair<float,float>* __restrict  bb[3] = {&xBB,&yBB,&zBB};

    // Inspired by pbrt's bbox intersection
    float t0 = r.near, t1 = r.far;
    for (int i = 0; i < 3; ++i) {
        float invRayDir = 1.f / r.direction[i];
        float tNear = ((*bb[i]).first  - r.origin[i]) * invRayDir;
        float tFar  = ((*bb[i]).second - r.origin[i]) * invRayDir;
        if (tNear > tFar) std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return res; // No intersection.
    }

    struct NodeToDo{
        const CompressedKdNode* node;
        float tmin, tmax;
    };


    glm::vec3 invDir(1.f/r.direction.x, 1.f/r.direction.y, 1.f/r.direction.z);

    NodeToDo todo[200];
    int todo_size = 1;
    todo[0] = NodeToDo{compressed_array, t0, t1};

    while(todo_size > 0){
        todo_size--;
        const CompressedKdNode* node = todo[todo_size].node;
        float tmin = todo[todo_size].tmin;
        float tmax = todo[todo_size].tmax;

        // Abort if we got too far.
        if(r.far < tmin) break;

        if(node->IsLeaf()){ // leaf node

            bool hit = false;
            // Search for intersections with triangles inside this node
            unsigned int n = node->GetTrianglesN();
            uint32_t tri_start = node->GetFirstTrianglePos();
            for(unsigned int p = 0; p < n; p++){
                // For each triangle ...
                unsigned int i = compressed_triangles[tri_start + p];
                const Triangle& tri = triangles[i];
                float t, a, b;

                // Skip the triangle if it matches ignore condition
                if(&tri == ignored) continue;

                //  ... test for an intersection
                if(tri.TestIntersection(r, t, a, b)){

                    // Skip the triangle, if the material is in thinglass set
                    const Material* mat = &(tri.GetMaterial());
                    if(mat->is_thinglass){
                        // Add this triangle data to intersection.
                        res.thinglass.push_back(std::make_pair(&tri,t));
                        // Skip.
                        continue;
                    }
                    if(t < tmin - epsilon || t > tmax + epsilon){
                        continue;
                    }
                    if(t < res.t){
                        // New closest intersect!
                        res.triangle = &tri;
                        res.t = t;

                        float c = 1.0f - a - b;
                        res.a = c;
                        res.b = a;
                        res.c = b;

                        hit = true;
                    }
                }
            }

            if(hit){
                return res;
            }

        }else{ // internal node

            // Compute parametric distance along ray to split plane
            int axis = node->GetSplitAxis();
            float tplane = (node->GetSplitPlane() - r.origin[axis]) * invDir[axis];

            // Get node children pointers for ray
            const CompressedKdNode *firstChild, *secondChild;
            int belowFirst = (r.origin[axis] <  node->GetSplitPlane()) ||
                             (r.origin[axis] == node->GetSplitPlane() && r.direction[axis] <= 0);
            if (belowFirst) {
                firstChild = node + 1;
                secondChild = compressed_array + node->GetOtherChildIndex();
            }else{
                firstChild = compressed_array + node->GetOtherChildIndex();
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tplane > tmax || tplane <= 0)
                todo[todo_size++] = NodeToDo{firstChild, tmin, tmax};
            else if (tplane < tmin)
                todo[todo_size++] = NodeToDo{secondChild, tmin, tmax};
            else {
                // Enqueue _secondChild_ in todo list
                todo[todo_size++] = NodeToDo{secondChild, tplane, tmax};
                todo[todo_size++] = NodeToDo{firstChild,  tmin, tplane};
            }
        }
    }

    // No hit found at all.
    return res;
}
