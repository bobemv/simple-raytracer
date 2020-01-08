#ifndef VEC3H
#define VEC3H
#include <math.h>
#include <iostream>

class vec3 {
public:
    double e[3];
public:
    vec3() {}
    vec3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double z() const { return e[2]; }

    inline double r() const { return e[0]; }
    inline double g() const { return e[1]; }
    inline double b() const { return e[2]; }

    //Single operand
    inline const vec3& operator +() const { return *this; }
    inline vec3 operator -() const { return vec3(-e[0], -e[1], -e[2]); }

    //Both operands
    inline vec3 operator +(const vec3 &v) const { return vec3(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]); };
    inline vec3 operator -(const vec3 &v) const { return vec3(e[0] - v.e[0], e[1] - v.e[1], e[2] - v.e[2]); };
    inline vec3 operator *(const vec3 &v) const { return vec3(e[0] * v.e[0], e[1] * v.e[1], e[2] * v.e[2]); };
    inline vec3 operator *(const int t) const { return vec3(t * e[0], t * e[1], t * e[2]); };
    inline vec3 operator *(const float t) const { return vec3(t * e[0], t * e[1], t * e[2]); };
    inline vec3 operator *(const double t) const { return vec3(t * e[0], t * e[1], t * e[2]); };
    inline vec3 operator /(const vec3 &v) const { return vec3(e[0] / v.e[0], e[1] / v.e[1], e[2] / v.e[2]); };
    inline vec3 operator /(const int t) const { return vec3(e[0] / t, e[1] / t, e[2] / t); };
    inline vec3 operator /(const float t) const { return vec3(e[0] / t, e[1] / t, e[2] / t); };
    inline vec3 operator /(const double t) const { return vec3(e[0] / t, e[1] / t, e[2] / t); };

    //Accesing vec3 class as array
    inline double operator [](int i) const { return e[i]; }
    inline double& operator [](int i) { return e[i]; }

    //Both operands (composite)
    inline vec3& operator +=(const vec3 &v) { e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2]; return *this; };
    inline vec3& operator -=(const vec3 &v) { e[0] -= v.e[0]; e[1] -= v.e[1]; e[2] -= v.e[2]; return *this; };
    inline vec3& operator *=(const vec3 &v) { e[0] *= v.e[0]; e[1] *= v.e[1]; e[2] *= v.e[2]; return *this; };
    inline vec3& operator *=(const int t) { e[0] *= t; e[1] *= t; e[2] *= t; return *this; };
    inline vec3& operator *=(const float t) { e[0] *= t; e[1] *= t; e[2] *= t; return *this; };
    inline vec3& operator *=(const double t) { e[0] *= t; e[1] *= t; e[2] *= t; return *this; };
    inline vec3& operator /=(const vec3 &v) { e[0] /= v.e[0]; e[1] /= v.e[1]; e[2] /= v.e[2]; return *this; };
    inline vec3& operator /=(const int t) { double k = 1.0/t; e[0] *= k; e[1] *= k; e[2] *= k; return *this; };
    inline vec3& operator /=(const float t) { double k = 1.0/t; e[0] *= k; e[1] *= k; e[2] *= k; return *this; };
    inline vec3& operator /=(const double t) { double k = 1.0/t; e[0] *= k; e[1] *= k; e[2] *= k; return *this; };

    //Vector operations
    inline double dot(const vec3 &v) const { return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2]; };
    inline vec3 cross(const vec3 &v) const {
        return vec3 (
            e[1] * v.e[2] - e[2] * v.e[1],
            - (e[0] * v.e[2] - e[2] * v.e[0]),
            e[0] * v.e[1] - e[1] * v.e[0]
        );
    };

    //Math utilities
    inline double length() const {
        return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
    }
    inline double squared_length() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
    inline void make_unit_vector() { float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); e[0] *= k; e[1] *= k; e[2] *= k; };
    inline vec3 unit_vector() const { return *this / this->length(); };

    //IO
    inline std::istream& operator >>(std::istream &is) { is >> e[0] >> e[1] >> e[2]; return is; };
    inline std::ostream& operator <<(std::ostream &os) { os << e[0] << " " << e[1] << " " << e[2]; return os; };
    inline void to_string() { printf("vec3(%f,%f,%f)\n", e[0], e[1], e[2]); };
};

inline vec3 operator *(const int t, const vec3& v) { return vec3(t * v.e[0], t * v.e[1], t * v.e[2]); };
inline vec3 operator *(const float t, const vec3& v) { return vec3(t * v.e[0], t * v.e[1], t * v.e[2]); };
inline vec3 operator *(const double t, const vec3& v) { return vec3(t * v.e[0], t * v.e[1], t * v.e[2]); };
inline vec3 operator /(const int t, const vec3& v) { return vec3(t / v.e[0], t / v.e[1], t / v.e[2]); };
inline vec3 operator /(const float t, const vec3& v) { return vec3(t / v.e[0], t / v.e[1], t / v.e[2]); };
inline vec3 operator /(const double t, const vec3& v) { return vec3(t / v.e[0], t / v.e[1], t / v.e[2]); };

#endif
