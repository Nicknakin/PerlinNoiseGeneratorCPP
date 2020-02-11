#include "perlin.h"

#include <vector>
#include <algorithm>
#include <iostream>

Perlin::Perlin(std::vector<int> dimensions) : dimensions{dimensions} {
    dimensionLengths = std::vector<int>{};
    dimensionLengths.resize(dimensions.size());
    for(int i = 0; i < dimensions.size(); i++){
        int temp = 1;
        for(int j = 0; j < i; j++){
            temp *= dimensions[j];
        }
        dimensionLengths[i] = temp;
    } 
}

float Perlin::operator()(std::vector<float> pos){
    auto shortenedPos = pos;
    std::transform(shortenedPos.begin(), shortenedPos.end(), shortenedPos.begin(), [](const float& coord) -> float {return coord-(float)((int) coord);});

    //Get Corners
    std::vector<int> topleft;
    std::transform(pos.begin(), pos.end(), topleft.begin(), [](float coord) -> int { return (int) coord;});
    auto bottomright = topleft;
     std::transform(pos.begin(), pos.end(), topleft.begin(), [](float coord) -> int { return (int) coord+1;});
    //Dot Products
     
    //Interpolations
    
    //return
    return 0.f;
}

std::vector<std::vector<int>> Perlin::combineArrays(std::vector<int> vec1, std::vector<int> vec2){
    std::vector<std::vector<int>> corners{};
    corners.resize(1);
    for(int i = 0; i < vec1.size(); i++){
        std::vector<std::vector<int>> newCorners{};
        for(int j = 0; j < corners.size(); j++){
            std::vector<int> temp1{corners[j]}, temp2{corners[j]};
            temp1.push_back(vec1[i]);
            temp2.push_back(vec2[i]);
            newCorners.push_back(temp1);
            newCorners.push_back(temp2);
        } 
        corners = newCorners;
    }
    return corners;
}


float Perlin::smoothstep(float start, float end, float val){
    return (val < start)? start : (val > end)? end: (3*val*val)-(2*val*val*val);
}
