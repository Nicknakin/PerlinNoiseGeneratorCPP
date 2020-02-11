#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iomanip>

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
    Perlin ng{std::vector<int>{10}};
    auto pos = std::vector<float>{0.12f, 0.13f, 2.3f};
    for(float i =0; i < 3.0f; i+=0.001f){
        std::cout << std::fixed << std::setw(3) << std::setprecision(2) << std::setfill('0') <<  i << ":\t";
        pos.resize(0);
        pos.push_back(i);
        int temp = ng(pos)*20;
        for(int k = 0; k<temp; k++)
            std::cout << "#";
        std::cout << std::endl;
    }

    return 0;
}


