#ifndef DIELECTRICH
#define DIELECTRICH

#include "material.h"
#include "vec3.h"
#include "randomize.h"

class dielectric : public material {
public:
    float ref_idx;
public:
    dielectric(float ri) : ref_idx(ri) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if (r_in.direction().dot(rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * r_in.direction().dot(rec.normal) / r_in.direction().length();
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -r_in.direction().dot(rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else {
            scattered = ray(rec.p, reflected);
            reflect_prob = 1.0;
        }
        randomize* randomize_gen = &randomize::get_instance();
        if (randomize_gen->get_random_float() < reflect_prob) {
            scattered = ray(rec.p, reflected);
        }
        else {
            scattered = ray(rec.p, refracted);
        }
        
        return true;
    }
private:
    vec3 reflect(const vec3& v, const vec3& n) const {
        return v - 2 * v.dot(n) * n;
    }
    bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) const {
        vec3 uv = v.unit_vector();
        float dt = uv.dot(n);
        float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
        if (discriminant > 0) {
            refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
            return true;
        }
        return false;
    }
    float schlick(float cosine, float ref_idx) const {
        float r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1-cosine), 5);
    }

};

#endif