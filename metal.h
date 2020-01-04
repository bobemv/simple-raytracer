#ifndef METALH
#define METALH

#include "material.h"
#include "randomize.h"

class metal : public material {
public:
    vec3 albedo;
public:
    metal(const vec3& a) : albedo(a) {};
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 reflected = reflect(r_in.direction().unit_vector(), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return scattered.direction().dot(rec.normal) > 0;
    }
private:
    vec3 reflect(const vec3& v, const vec3& n) const {
        return v - 2 * v.dot(n) * n;
    }
};

#endif