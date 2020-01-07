#ifndef METALH
#define METALH

#include "material.h"
#include "randomize.h"

class metal : public material {
public:
    vec3 albedo;
    float fuzz;
public:
    metal(const vec3& a) : albedo(a) { fuzz = 0; };
    metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; };
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        randomize* randomize_gen = &randomize::get_instance();
        vec3 reflected = reflect(r_in.direction().unit_vector(), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*randomize_gen->random_in_unit_sphere());
        attenuation = albedo;
        return scattered.direction().dot(rec.normal) > 0;
    }
private:
    vec3 reflect(const vec3& v, const vec3& n) const {
        return v - 2 * v.dot(n) * n;
    }
};

#endif