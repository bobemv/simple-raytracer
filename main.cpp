#include <fstream>
#include <iostream>
#include <ctime>
#include <cstring>
#include "vec3.h"

using namespace std;

int main()
{
    ofstream myfile;
    time_t t = time(nullptr);
    asctime(localtime(&t));
    char filename[50] = "example_";
    char timestamp[30];
    itoa(t, timestamp, 10);
    strcat(filename, timestamp);
    strcat(filename, ".ppm");
    myfile.open(filename);


    int nx = 200;
    int ny = 100;
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
    return 0;
}