#ifndef LAMBERTIANH
#define LAMBERTIANH

#include "material.h"
#include "randomize.h"

class lambertian : public material {
public:
    vec3 albedo;
public:
    lambertian(const vec3& a) : albedo(a) {};
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        randomize* randomize_gen = &randomize::get_instance();
        vec3 target = rec.p + rec.normal + randomize_gen->random_in_unit_sphere();
        scattered = ray(rec.p, target-rec.p);
        attenuation = albedo;
        return true;
    }
};

#endif