#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <math.h>

#include "perlin.h"

std::ostream& operator<<(std::ostream& os, const std::vector<int> &input)
{
        for (auto const& i: input) {
                    os << i << ", ";
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
    const int width = 100;
    outs.resize(width*width/boxSize/boxSize);
    for(int i = 0; i < width*width/boxSize/boxSize; i++){
        auto pos = std::vector<float>{(float) (i%(width/boxSize))/(float) width/boxSize * (float) 25, (float) std::floor(i/(width/boxSize))/(float) width/boxSize * (float) 25, 0.f};
        outs[i] = ng(pos)*255;
    } 
    std::cout << outs << std::endl;

    return 0;
}


