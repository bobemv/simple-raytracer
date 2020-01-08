#ifndef M_PI
#define M_PI 3.14159265359
#endif

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

#include "triangle.h"
#include "mesh.h"

#include "zlib/zlib.h"
#include <string.h>

using namespace std;

//Chapter 12
hitable *random_scene() {
    int n = 500;
    randomize* randomize_gen = &randomize::get_instance();
    hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = randomize_gen->get_random_float();
            vec3 center(a+0.9*randomize_gen->get_random_float(), 0.2, b + 0.9*randomize_gen->get_random_float());
            if ((center - vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(randomize_gen->get_random_float()*randomize_gen->get_random_float(), randomize_gen->get_random_float()*randomize_gen->get_random_float(), randomize_gen->get_random_float()*randomize_gen->get_random_float())));
                }
                else if (choose_mat < 0.95) {
                    list[i++] = new sphere(center, 0.2,
                                new metal(vec3(0.5*(1 + randomize_gen->get_random_float()), 0.5*(1 + randomize_gen->get_random_float()), 0.5*(1 + randomize_gen->get_random_float())), 0.5 * randomize_gen->get_random_float()));
                }
                else {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list, i);
}

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

// Chapter 12
void create_random_scene(int width, int height) {
    
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));

    char filename[100] = "randomScene_";
    char timestamp[30];
    sprintf(timestamp, "%ld", t);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 2;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    float R = cos(M_PI/4);
    hitable *world = random_scene();

    vec3 lookfrom(15, 1, 4);
    vec3 lookat(0,1,0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 2;
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx) / float(ny), aperture, dist_to_focus);
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

// Chapter 11
void create_gradient_with_sphere_shaded_antialiasing_materials_camera_blur_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasingMaterialsCameraBlur_";
    char timestamp[30];
    sprintf(timestamp, "%ld", t);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 200;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    float R = cos(M_PI/4);
    hitable *list[5];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2)));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    hitable *world = new hitable_list(list, 5);

    vec3 lookfrom(3, 3, 2);
    vec3 lookat(0,0,-1);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 2.0;
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx) / float(ny), aperture, dist_to_focus);
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

// Chapter 10
void create_gradient_with_sphere_shaded_antialiasing_materials_camera_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasingMaterialsCamera_";
    char timestamp[30];
    sprintf(timestamp, "%ld", t);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 200;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";

        float R = cos(M_PI/4);
    hitable *list[5];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2)));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    hitable *world = new hitable_list(list, 5);
    camera cam(vec3(-2, 2, 1), vec3(0,0,-1), vec3(0,1,0), 90, float(nx) / float(ny));
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


// Chapter 9
void create_gradient_with_sphere_shaded_antialiasing_materials_2_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasingMaterials2_";
    char timestamp[30];
    sprintf(timestamp, "%ld", t);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 200;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    
    hitable *list[5];
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
// Mesh
void create_mesh_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "mesh";
    char timestamp[30];
    sprintf(timestamp, "%ld", t);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = width;
    int ny = height;
    int ns = 40;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *list[4];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    //list[3] = new sphere(vec3(-1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 1.0));
    triangle* triangles[4];

    triangles[0] = new triangle(vec3(-0.5, -0.5, 0.25), vec3(0.5, 0.5, 0.25), vec3(0.5, -0.5, 0.25));
    triangles[1] = new triangle(vec3(-0.5, 0.5, 0.25), vec3(0.5, 0.5, 0.25), vec3(-0.5, -0.5, 0.25));

    triangles[2] = new triangle(vec3(0.5, 10.5, 10.25), vec3(0.5, 10.5, -10.25), vec3(0.5, -0.5, 10.25));
    triangles[3] = new triangle(vec3(0.5, -0.5, 10.25), vec3(0.5, 10.5, -10.25), vec3(0.5, -0.5, -10.25));
    
    list[3] = new mesh(vec3(-1, 0, -1), triangles, 4, new metal(vec3(0.8, 0.8, 0.8), 0.0));
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

// Chapter 8
void create_gradient_with_sphere_shaded_antialiasing_materials_rays_image(int width, int height)
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[100] = "gradientRaysWithSphereShadedAntialiasingMaterials_";
    char timestamp[30];
    sprintf(timestamp, "%ld", t);
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
    sprintf(timestamp, "%ld", t);
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
    sprintf(timestamp, "%ld", t);
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
    sprintf(timestamp, "%ld", t);
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
    sprintf(timestamp, "%ld", t);
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
    sprintf(timestamp, "%ld", t);
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
    sprintf(timestamp, "%ld", t);
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
    sprintf(timestamp, "%ld", t);
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

int check_zlib()
{   
    // original string len = 36
    char a[50] = "Hello Hello Hello Hello Hello Hello!"; 

    // placeholder for the compressed (deflated) version of "a" 
    char b[50];

    // placeholder for the UNcompressed (inflated) version of "b"
    char c[50];
     

    printf("Uncompressed size is: %lu\n", strlen(a));
    printf("Uncompressed string is: %s\n", a);


    printf("\n----------\n\n");

    // STEP 1.
    // deflate a into b. (that is, compress a into b)
    
    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    // setup "a" as the input and "b" as the compressed output
    defstream.avail_in = (uInt)strlen(a)+1; // size of input, string + terminator
    defstream.next_in = (Bytef *)a; // input char array
    defstream.avail_out = (uInt)sizeof(b); // size of output
    defstream.next_out = (Bytef *)b; // output char array
    
    // the actual compression work.
    deflateInit(&defstream, Z_BEST_COMPRESSION);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);
     
    // This is one way of getting the size of the output
    printf("Compressed size is: %lu\n", strlen(b));
    printf("Compressed string is: %s\n", b);
    

    printf("\n----------\n\n");


    // STEP 2.
    // inflate b into c
    // zlib struct
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    // setup "b" as the input and "c" as the compressed output
    infstream.avail_in = (uInt)((char*)defstream.next_out - b); // size of input
    infstream.next_in = (Bytef *)b; // input char array
    infstream.avail_out = (uInt)sizeof(c); // size of output
    infstream.next_out = (Bytef *)c; // output char array
     
    // the actual DE-compression work.
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
     
    printf("Uncompressed size is: %lu\n", strlen(c));
    printf("Uncompressed string is: %s\n", c);
    

    // make sure uncompressed is exactly equal to original.
    //assert(strcmp(a,c)==0);

    return 0;
}

int read_mesh()
{   
    /*FILE *dic;
    dic = fopen("cube.fbx","rb");
    if (dic == NULL) {
        printf("error fopen\n");
    }
    else {
        printf("bien fopen\n");
        char buffersad[10];
        fgets(buffersad, 10, dic );
        printf("%s\n", buffersad);
    }

    fclose(dic);*/

    ifstream input( "cube.fbx", std::ios::binary | std::ios::in |  std::ios::ate);

    if(input.is_open())
    {
        printf("Is_open\n");
    }
    else{
        printf("Not is_open\n");
    }

    if(input.good())
    {
        printf("good\n");
    }
    else{
        printf("Not good\n");
    }


    int size = input.tellg();
    input.seekg(0, ios::beg);
    vector<char> buffer;
    buffer.resize(size); // << resize not reserve
    input.read(buffer.data(), size);

    //cout.write(buffer.data(), buffer.size()); 

    //std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    char vertices_name[9] = "Vertices";
    int n_vertices_name = 8;
    
    
    int n_find = 0;
    long buffer_index_vertices = 0;
    //printf("Empieza\n");
    //printf("Size: %d\n", size);
    for(long int buffer_index = 0; buffer_index < buffer.size(); buffer_index++) {
        //printf("%ld - %c\n", buffer_index, buffer.data()[buffer_index]);
        if (buffer.data()[buffer_index] == vertices_name[n_find]) {
            //printf("Found one! - i: %d - index: %ld\n", n_find, buffer_index);
            n_find++;
        }
        else {
            n_find = 0;
        }
        if (n_find == 7) {
            printf("Vertices found! At byte position: %ld\n", buffer_index);
            buffer_index_vertices = buffer_index + 1;
        }
    }
    buffer_index_vertices += 1; //Skip array type
    char *int_in_chars;
    int_in_chars = new char[4];

    int n_array_vertices_bytes = 0;
    input.seekg (buffer_index_vertices, ios::beg);
    input.read(int_in_chars, 4);
    printf("Read: %s\n", int_in_chars);
    //sscanf(int_in_chars, "%d", &n_array_vertices_bytes);
    n_array_vertices_bytes += (int)int_in_chars[4];
    n_array_vertices_bytes = n_array_vertices_bytes << 4;
    n_array_vertices_bytes += (int)int_in_chars[3];
    n_array_vertices_bytes = n_array_vertices_bytes << 4;
    n_array_vertices_bytes += (int)int_in_chars[2];
    n_array_vertices_bytes = n_array_vertices_bytes << 4;
    n_array_vertices_bytes += (int)int_in_chars[1];
    buffer_index_vertices += 4; //Skip length vertices bytes

    buffer_index_vertices += 4; //Skip encoding

    int n_array_compressed_vertices_bytes = 0;
    input.seekg (buffer_index_vertices, ios::beg);
    input.read(int_in_chars, 4);
    printf("Read: %s\n", int_in_chars);

    //sscanf(int_in_chars, "%d", &n_array_compressed_vertices_bytes);
    n_array_compressed_vertices_bytes += (int)int_in_chars[4];
    n_array_compressed_vertices_bytes = n_array_compressed_vertices_bytes << 4;
    n_array_compressed_vertices_bytes += (int)int_in_chars[3];
    n_array_compressed_vertices_bytes = n_array_compressed_vertices_bytes << 4;
    n_array_compressed_vertices_bytes += (int)int_in_chars[2];
    n_array_compressed_vertices_bytes = n_array_compressed_vertices_bytes << 4;
    n_array_compressed_vertices_bytes += (int)int_in_chars[1];

    buffer_index_vertices += 4; //Skip length vertices bytes compressed
    input.seekg (buffer_index_vertices, ios::beg);

    printf("Compressed length bytes: %d\n", n_array_compressed_vertices_bytes);
    printf("Uncompressed length bytes: %d\n", n_array_vertices_bytes);

    char * compressedArray;
    compressedArray = new char[n_array_compressed_vertices_bytes];

    char * uncompressedArray;
    uncompressedArray = new char[n_array_vertices_bytes];

    input.read(compressedArray, n_array_compressed_vertices_bytes);

    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    // setup "b" as the input and "c" as the compressed output
    infstream.avail_in = n_array_compressed_vertices_bytes; // size of input
    infstream.next_in = (Bytef *)compressedArray; // input char array
    infstream.avail_out = n_array_vertices_bytes; // size of output
    infstream.next_out = (Bytef *)uncompressedArray; // output char array
     
    // the actual DE-compression work.
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);

    for (int i = 0; i < (n_array_vertices_bytes / 8); i++)
    {
        char * char_ptr;
        char_ptr = new char[8];
        char_ptr = uncompressedArray + 8 * i;
        double vertice = atof(char_ptr);
       

        printf("Double: %f\n", vertice);

    }

    return 0;
}

int main()
{
    int nx = 200;
    int ny = 100;
    //create_mesh_image(nx, ny);
    read_mesh();
}
