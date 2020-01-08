#ifndef TRIANGLEH
#define TRIANGLEH

#include "vec3.h"
#include "ray.h"
// Functions to check triangle intersect in https://stackoverflow.com/questions/42740765/intersection-between-line-and-triangle-in-3d
class triangle {
public:
    vec3 p1;
    vec3 p2;
    vec3 p3;
    vec3 normal;
public:
    triangle(vec3 a, vec3 b, vec3 c) {
        p1 = a;
        p2 = b;
        p3 = c;
        normal = (p2 - p3).cross(p1 - p3).unit_vector();
    };
    triangle(vec3 a, vec3 b, vec3 c, vec3 n) {
        p1 = a;
        p2 = b;
        p3 = c;
        normal = n;
    };
    bool get_point_intersection(const vec3& offset, const ray& r, float& t_in) {
        if (is_intersection(offset, r)) {
            float t = (-(r.origin() - (p1 + offset)).dot(normal)) / r.direction().dot(normal);
            t_in = t;
            return true;
        }
        return false;
    };
    vec3 get_normal() {
        return normal;
    };
    void to_string() {
        printf("triangle\n");
        p1.to_string();
        p2.to_string();
        p3.to_string();
        printf("\n");
    };
private:
    bool is_intersection(const vec3& offset, const ray& r) {
        vec3 q1 = r.point_at_parameter(-1000);
        vec3 q2 = r.point_at_parameter(1000);
        vec3 p1_offset = p1 + offset;
        vec3 p2_offset = p2 + offset;
        vec3 p3_offset = p3 + offset;
        int first_tetrahedron_sign = signed_volume(q1, p1_offset, p2_offset, p3_offset);
        int second_tetrahedron_sign = signed_volume(q2, p1_offset, p2_offset, p3_offset);

        int third_tetrahedron_sign = signed_volume(q1, q2, p1_offset, p2_offset);
        int fourth_tetrahedron_sign = signed_volume(q1, q2, p2_offset, p3_offset);
        int fifth_tetrahedron_sign = signed_volume(q1, q2, p3_offset, p1_offset);

        bool first_condition = (first_tetrahedron_sign >= 0 && second_tetrahedron_sign < 0) ||
                               (first_tetrahedron_sign < 0 && second_tetrahedron_sign >= 0);
        bool second_condition = (third_tetrahedron_sign >= 0 && fourth_tetrahedron_sign >= 0 && fifth_tetrahedron_sign >= 0) ||
                                (third_tetrahedron_sign < 0 && fourth_tetrahedron_sign < 0 && fifth_tetrahedron_sign < 0);
        
        return first_condition && second_condition;
    }

    int signed_volume(const vec3& a, const vec3& b, const vec3& c, const vec3& d) {
        return ((b-a).cross(c-a)).dot(d-a);
    }

};


#endif