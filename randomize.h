#ifndef RANDOMIZEH
#define RANDOMIZEH

#include <cstdlib>
#include <random>

using namespace std;

class randomize {

public:
    random_device rd;
    mt19937 gen;
    uniform_real_distribution<float> generator;
public:
    randomize() {
        random_device rd;
        gen = mt19937(rd());
        generator = uniform_real_distribution<float>(0, 1.0);
    }
    inline float get_random_float() { return generator(gen); };
};
#endif