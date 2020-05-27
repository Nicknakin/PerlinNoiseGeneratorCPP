#ifndef __perlin_inc__
#define __perlin_inc__
#include <vector>

class Perlin {
public:
    Perlin();
    Perlin(std::initializer_list<int> dims);
    Perlin(std::vector<int> dimensions);
    Perlin(std::vector<int> dimensions, int octaves);
    Perlin(std::vector<int> dimensions, int octaves, int seed);
    double operator()(std::vector<double> pos);
    double noise(std::vector<double> pos);

    std::vector<std::vector<int>> combineArrays(std::vector<int> vec1, std::vector<int> vec2);
    double interp(double start, double end, double val);
    double smoothstep(double start, double end, double val);
    double smoothstepinterp(double start, double end, double val);
    double dot(std::vector<double> vec1, std::vector<double> vec2);

    std::vector<int> nodes;
    std::vector<int> dimensions;
    std::vector<int> dimensionLengths;
    std::vector<std::vector<int>> subVector;
    std::vector<std::vector<double>> baseVectors;
    
    int octaves;
    const double range;
    double octAdjust;
    double reductionBase;
    std::vector<double> divisors;
};

#endif

