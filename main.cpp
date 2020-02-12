#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <math.h>

#include "perlin.h"

std::vector<std::vector<int>> combineArrays(std::vector<int> vec1, std::vector<int> vec2){
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

std::ostream& operator<<(std::ostream& os, const std::vector<int> &input)
{
        for (auto const& i: input) {
                    os << i << " ";
                        }
            return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<int>> &input)
{
        for (auto const& i: input) {
                    os << i << " ";
                        }
            return os;
}

int main(int argc, char** args){
    Perlin ng{std::vector<int>{255, 255, 10}};
    std::vector<int> outs{};
    const int boxSize = 1;
    outs.resize(800*800/boxSize/boxSize);
    for(int i = 0; i < 800*800/boxSize/boxSize; i++){
        auto pos = std::vector<float>{(float) (i%(800/boxSize))/(float) 800/boxSize * (float) 50, (float) std::floor(i/(800/boxSize))/(float) 800/boxSize * (float) 50, 0.f};
        outs[i] = ng(pos)*255;
    } 
    std::cout << outs << std::endl;

    return 0;
}


