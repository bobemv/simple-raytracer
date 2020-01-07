#include <fstream>
#include <iostream>
#include <ctime>
#include <cstring>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "hitable.h"
#include "camera.h"
#include "randomize.h"
#include "material.h"
#include "metal.h"
#include "lambertian.h"
#include "dielectric.h"

using namespace std;

// Chpater 7
vec3 random_in_unit_sphere() {
    vec3 p;
    vec3 p_randomized;
    randomize* randomize_gen = &randomize::get_instance();
    do {
        p_randomized = vec3(randomize_gen->get_random_float(), randomize_gen->get_random_float(), randomize_gen->get_random_float());
        p = 2.0 * p_randomized - vec3(1,1,1);
    } while (p.squared_length() > 1.0);
    return p;
}
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

// Chapter 5 - 2
vec3 color_gradient_with_sphere_shaded_2(const ray& r, hitable *world) {
    hit_record rec;
    if (world->hit(r, 0.0, FLT_MAX, rec)) {
        return 0.5*vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
    }

    vec3 unit_direction = r.direction().unit_vector();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

// Chapter 7
vec3 color_gradient_with_sphere_shaded_diffused(const ray& r, hitable *world) {
    hit_record rec;
    randomize* randomize_gen = &randomize::get_instance();
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        vec3 target = rec.p + rec.normal + randomize_gen->random_in_unit_sphere();
        return 0.5*color_gradient_with_sphere_shaded_diffused(ray(rec.p, target-rec.p), world);
    }

    vec3 unit_direction = r.direction().unit_vector();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

// Chapter 8
vec3 color_gradient_with_sphere_shaded_materials(const ray& r, hitable *world, int depth) {
    hit_record rec;
    randomize* randomize_gen = &randomize::get_instance();
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr != nullptr && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color_gradient_with_sphere_shaded_materials(scattered, world, depth + 1);
        }
        else {
            return vec3(0,0,0);
        }
    }

    vec3 unit_direction = r.direction().unit_vector();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

// Chapter 9
void create_gradient_with_sphere_shaded_antialiasing_materials_2_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasingMaterials2_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 200;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    hitable *list[4];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2)));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    hitable *world = new hitable_list(list, 5);
    camera cam;
    randomize* randomize_gen = &randomize::get_instance();
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0,0,0);
            
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + randomize_gen->get_random_float()) / float(nx);
                float v = float(j + randomize_gen->get_random_float()) / float(ny);
                ray r = cam.get_ray(u, v);
                //vec3 p = r.point_at_parameter(2.0);
                col += color_gradient_with_sphere_shaded_materials(r, world, 0);
            }

            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}

// Chapter 8
void create_gradient_with_sphere_shaded_antialiasing_materials_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasingMaterials_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 200;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    hitable *list[4];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 1.0));
    hitable *world = new hitable_list(list, 4);
    camera cam;
    randomize* randomize_gen = &randomize::get_instance();
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0,0,0);
            
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + randomize_gen->get_random_float()) / float(nx);
                float v = float(j + randomize_gen->get_random_float()) / float(ny);
                ray r = cam.get_ray(u, v);
                //vec3 p = r.point_at_parameter(2.0);
                col += color_gradient_with_sphere_shaded_materials(r, world, 0);
            }

            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}

// Chapter 7
void create_gradient_with_sphere_shaded_antialiasing_diffused_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasingDiffused_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 200;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    hitable *list[2];
    list[0] = new sphere(vec3(0, 0, -1), 0.5);
    list[1] = new sphere(vec3(0, -100.5, -1), 100);
    hitable *world = new hitable_list(list, 2);
    camera cam;
    randomize* randomize_gen = &randomize::get_instance();
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0,0,0);
            
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + randomize_gen->get_random_float()) / float(nx);
                float v = float(j + randomize_gen->get_random_float()) / float(ny);
                ray r = cam.get_ray(u, v);
                //vec3 p = r.point_at_parameter(2.0);
                col += color_gradient_with_sphere_shaded_diffused(r, world);
            }

            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}

// Chapter 6
void create_gradient_with_sphere_shaded_antialiasing_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasing_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 200;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    hitable *list[2];
    list[0] = new sphere(vec3(0, 0, -1), 0.5);
    list[1] = new sphere(vec3(0, -100.5, -1), 100);
    hitable *world = new hitable_list(list, 2);
    camera cam;
    randomize* randomize_gen = &randomize::get_instance();
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0,0,0);
            
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + randomize_gen->get_random_float()) / float(nx);
                float v = float(j + randomize_gen->get_random_float()) / float(ny);
                ray r = cam.get_ray(u, v);
                //vec3 p = r.point_at_parameter(2.0);
                col += color_gradient_with_sphere_shaded_2(r, world);
            }

            col /= float(ns);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}
// Chapter 5 - 2
void create_gradient_with_sphere_shaded_2_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[50] = "gradientRaysWithSphereShaded2_";
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
    hitable *list[2];
    list[0] = new sphere(vec3(0, 0, -1), 0.5);
    list[1] = new sphere(vec3(0, -100.5, -1), 100);
    hitable *world = new hitable_list(list, 2);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            vec3 p = r.point_at_parameter(2.0);
            vec3 col = color_gradient_with_sphere_shaded_2(r, world);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
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
    create_gradient_with_sphere_shaded_antialiasing_materials_2_rays_image(nx, ny);
}