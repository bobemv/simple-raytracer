#ifndef MESHH
#define MESHH

#include "hitable.h"
#include "triangle.h"
#include "material.h"

class mesh : public hitable {
public:
    vec3 center;
    triangle** triangles;
    int ntriangles;
    material* material_ptr;
public:
    mesh(vec3 ctr, triangle** tris, int n, material* mat_ptr) {
        center = ctr;
        triangles = tris;
        ntriangles = n;
        material_ptr = mat_ptr;
    }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
};

bool mesh::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {

    bool any_triangle_hit = false;
    for (int i = 0; i < ntriangles; i++) {
        vec3 intersection;
        float temp;
        double closest_so_far = t_max;
        if (triangles[i]->get_point_intersection(center, r, temp)) {
            if (temp < t_max && temp > t_min && closest_so_far > temp) {
                rec.t = temp;
                rec.p = r.point_at_parameter(rec.t);
                rec.normal = triangles[i]->get_normal();
                rec.mat_ptr = material_ptr;
                any_triangle_hit = true;
                closest_so_far = temp;
            }
        }
    }

    return any_triangle_hit;
}

#endif