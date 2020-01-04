#ifndef RANDOMIZEH
#define RANDOMIZEH

#include <cstdlib>
#include <random>
#include "vec3.h"

using namespace std;

class randomize {

public:
    static random_device rd;
    static mt19937 gen;
    static uniform_real_distribution<float> generator;
private:
    //static randomize* instance;
    randomize() {
        randomize::gen = mt19937(randomize::rd());
        randomize::generator = uniform_real_distribution<float>(0, 1.0);
    };
    randomize(randomize const&);
    void operator=(randomize const&);
public:
    static inline randomize& get_instance() { static randomize instance; return instance; };
    static inline float get_random_float() { return randomize::generator(randomize::gen); };
    static vec3 random_in_unit_sphere() {
        vec3 p;
        vec3 p_randomized;
        do {
            p_randomized = vec3(get_random_float(), get_random_float(), get_random_float());
            p = 2.0 * p_randomized - vec3(1,1,1);
        } while (p.squared_length() > 1.0);
        return p;
    };
};

random_device randomize::rd;
mt19937 randomize::gen;
uniform_real_distribution<float> randomize::generator;
#endif