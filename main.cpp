#include <fstream>
#include <iostream>
#include <ctime>
#include <cstring>
#include "vec3.h"
#include "ray.h"

using namespace std;

// Chapter 4
bool hit_sphere(const vec3& center, float radius, const ray& r) {
    vec3 oc = r.origin() - center;
    float a = r.direction().dot(r.direction());
    float b = 2.0 * oc.dot(r.direction());
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

// Chapter 5
float hit_sphere_v2(const vec3& center, float radius, const ray& r) {
    vec3 oc = r.origin() - center;
    float a = r.direction().dot(r.direction());
    float b = 2.0 * oc.dot(r.direction());
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return -1.0;
    }
    else {
        return (-b - sqrt(discriminant) / (2.0 * a));
    }
}

// Chapter 3
vec3 color_gradient(const ray& r) {
    vec3 unit_direction = r.direction().unit_vector();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + vec3 (0.5, 0.7, 1.0) * t;
}

// Chapter 4
vec3 color_gradient_with_sphere(const ray& r) {
    if (hit_sphere(vec3(0,0,-1), 0.5, r))
        return vec3(1, 0, 0);
    vec3 unit_direction = r.direction().unit_vector();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + vec3 (0.5, 0.7, 1.0) * t;
}

// Chapter 5
vec3 color_gradient_with_sphere_shaded(const ray& r) {
    float t = hit_sphere_v2(vec3(0,0,-1), 0.5, r);
    if (t > 0.0)
    {
        vec3 N = (r.point_at_parameter(t) - vec3(0,0,-1)).unit_vector();
        return 0.5 * vec3(N.x() + 1, N.y() + 1, N.z() + 1);
    }
    vec3 unit_direction = r.direction().unit_vector();
    t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + vec3 (0.5, 0.7, 1.0) * t;
}

// Chapter 5
void create_gradient_with_sphere_shaded_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[50] = "gradientRaysWithSphereShaded_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            vec3 col = color_gradient_with_sphere_shaded(r);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}

// Chapter 4
void create_gradient_with_sphere_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[50] = "gradientRaysWithSphere_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            vec3 col = color_gradient_with_sphere(r);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}

// Chapter 3
void create_gradient_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[50] = "gradientRays_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            vec3 col = color_gradient(r);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}

// Chapter 2
void create_gradient_vectors_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[50] = "gradientVectors_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(float(i) / float(nx), float(j) / float(ny), 0.2);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}

int main()
{
    int nx = 200;
    int ny = 100;
    create_gradient_with_sphere_shaded_rays_image(nx, ny);
}