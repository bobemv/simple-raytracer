#ifndef CAMERAH
#define CAMERAH
#ifndef M_PI
#define M_PI 3.14159265359
#endif
#include "ray.h"
#include "randomize.h"

class camera {
    public:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        float lens_radius;
    public:
        camera() {
            lower_left_corner = vec3(-2.0, -1.0, -1.0);
            horizontal = vec3(4.0, 0.0, 0.0);
            vertical = vec3(0.0, 2.0, 0.0);
            origin = vec3(0.0, 0.0, 0.0);
        }
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
            lens_radius = aperture / 2;
            float theta = vfov*M_PI/180;
            float half_height = tan(theta / 2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = (lookfrom - lookat).unit_vector();
            u = (vup.cross(w)).unit_vector();
            v = w.cross(u);
            lower_left_corner = vec3(-half_width, -half_height, -1.0);
            lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist*w;
            horizontal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
        }
        ray get_ray(float s, float t) {
            randomize* randomize_gen = &randomize::get_instance();
            vec3 rd = lens_radius * randomize_gen->random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
        }
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
            vec3 u, v, w;
            float theta = vfov*M_PI/180;
            float half_height = tan(theta / 2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = (lookfrom - lookat).unit_vector();
            u = (vup.cross(w)).unit_vector();
            v = w.cross(u);
            lower_left_corner = vec3(-half_width, -half_height, -1.0);
            lower_left_corner = origin - half_width*u - half_height*v - w;
            horizontal = 2*half_width*u;
            vertical = 2*half_height*v;
        }
};

#endif