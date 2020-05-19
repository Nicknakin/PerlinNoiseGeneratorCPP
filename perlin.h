#ifndef __perlin_inc__
#define __perlin_inc__
#include <vector>

class Perlin {
public:
    Perlin();
    Perlin(std::initializer_list<int> dims);
    Perlin(std::vector<int> dimensions);
    Perlin(std::vector<int> dimensions, int seed);
    float operator()(std::vector<float> pos);
    float noise(std::vector<float> pos);

    std::vector<std::vector<int>> combineArrays(std::vector<int> vec1, std::vector<int> vec2);
    float interp(float start, float end, float val);
    float smoothstep(float start, float end, float val);
    float smoothstepinterp(float start, float end, float val);
    float dot(std::vector<float> vec1, std::vector<int> vec2);

    std::vector<int> nodes;
    std::vector<int> dimensions;
    std::vector<int> dimensionLengths;
    std::vector<std::vector<int>> subVector;
    std::vector<std::vector<int>> baseVectors;
    const float range;
    
    int octaves;
    float octAdjust;
    float reductionBase;
};
#endif

