#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "ray.h"
#include "vec3.h"
#include "material.h"

class sphere: public hitable {
    public:
        vec3 center;
        float radius;
        material* material_ptr;
    public:
        sphere() {}
        sphere(vec3 cen, float r) : center(cen), radius(r){}; // CHAPTERS < 8
        sphere(vec3 cen, float r, material* mat_ptr) : center(cen), radius(r), material_ptr(mat_ptr) {};
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = r.direction().dot(r.direction());
    float b = oc.dot(r.direction());
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0)
    {
        float temp = (-b - sqrt(b * b - a * c)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = material_ptr;
            return true;
        }
        temp = (-b + sqrt(b * b - a * c)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = material_ptr;
            return true;
        }
    }
    return false;
}

#endif