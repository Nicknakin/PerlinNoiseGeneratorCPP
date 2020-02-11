#ifndef perlin_def
#define perlin_def

#include <vector>

class Perlin{
    public:
        Perlin(std::vector<int> dimensions);
        float operator()(std::vector<float> pos);

    private:
        std::vector<std::vector<int>> combineArrays(std::vector<int> vec1, std::vector<int> vec2); 
        float interp(float start, float end, float val);
        float smoothstep(float start, float end, float val);

        std::vector<std::vector<float>> nodes;
        std::vector<int> dimensions;
        std::vector<int> dimensionLengths;
};

#endif
