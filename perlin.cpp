#include "perlin.h"

#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>

Perlin::Perlin(std::vector<int> dimensions) : dimensions{dimensions} {
    //Initialize an empty vector of length dimensions.size()
    dimensionLengths = std::vector<int>{};
    dimensionLengths.resize(dimensions.size());
    //Iterate through vector replacing each value with the product of all values before it(Can absolutely be replaced with a transform and reduce)
    for(int i = 0; i < dimensions.size(); i++){
        int baseProd = 1;
        //This should be a reduce but I don't know how to do that, but it's acc*val with default of 1
        for(int j = 0; j < i; j++){
            baseProd *= dimensions[j];
        }
        dimensionLengths[i] = baseProd;
    }

    //Base direction defaults to create a uniform 3^n-1 number of vectors pointing from the center out ward (will remove the {0,0,...0} value at end
    std::vector<int> BASES{0, 1, -1}; 
    //Initialized spacedVectors as an empty vector to be expanded on
    spacedVectors = std::vector<std::vector<int>>{std::vector<int>{}};

    //Iterate for the number of dimensions
    for(int i = 0; i < dimensions.size(); i++){
        //Define a vector to fill with the expansion from spacedVectors and BASES
        std::vector<std::vector<int>> expandedVec{};
        for(int j = 0; j < spacedVectors.size(); j++){
            //For each value in BASES do a push_back on the current value in spacedVectors and push it to expandedVec (Turn every value in spacedVectors in to three new values)
            for(int k = 0; k < 3; k++){
                auto temp = spacedVectors[j];
                temp.push_back(BASES[k]);
                expandedVec.push_back(temp);
            }
        }
        //Replace spacedVectors with its expanded version for next iteration
        spacedVectors = expandedVec;
    }
    
     
    //Remove the {0,0... 0} value which is always on top
    spacedVectors.erase(spacedVectors.begin()); 

    //Populate nodes
    std::srand(142857);
    int numNodes = 1;
    for(auto const &val : dimensions)
        numNodes *= val;
    nodes = std::vector<int>{};
    nodes.resize(numNodes);
    std::transform(nodes.begin(), nodes.end(), nodes.begin(), [&](auto _) -> int { return std::rand()%spacedVectors.size();});

    subVector = combineArrays(std::vector<int>{0,0,0}, std::vector<int>{1,1,1});
}

float Perlin::operator()(std::vector<float> pos){

    //Get 0-1 version of position with whole number cut off
    std::vector<float> shortenedPos;
    shortenedPos.resize(pos.size());
    std::transform(pos.begin(), pos.end(), shortenedPos.begin(), [](const float& coord) -> float {return coord-floor(coord);});
    
    //Get Corners
    std::vector<int> topleft;
    topleft.resize(dimensions.size());
    std::transform(pos.begin(), pos.end(), topleft.begin(), [](float coord) -> int { return (int) coord;});
    auto bottomright = topleft;
     std::transform(pos.begin(), pos.end(), bottomright.begin(), [](float coord) -> int { return (int) coord+1;});
    auto corners = combineArrays(topleft, bottomright);
    std::vector<std::vector<int>> cornerVectors{};
    cornerVectors.resize(corners.size());
    std::transform(corners.begin(), corners.end(), cornerVectors.begin(), 
        [&](auto corner) -> std::vector<int> { 
            for(int i = 0; i < corner.size(); i++){
                corner[i] *= dimensionLengths[i];
            }    
            return spacedVectors[nodes[std::accumulate(corner.begin(), corner.end(), 0)]];
        }); 

    //Dot Products
    std::vector<std::vector<float>> distFromCorner{};
    distFromCorner.resize(cornerVectors.size(), shortenedPos);
    for(int i = 0; i < distFromCorner.size(); i++){
        for(int j = 0; j < distFromCorner[i].size(); j++){
            distFromCorner[i][j] -= subVector[i][j];
        }
    }
    
    std::vector<float> dotProds{};
    dotProds.resize(cornerVectors.size());
    for(int i = 0; i < dotProds.size(); i++){
        dotProds[i] = dot(distFromCorner[i], cornerVectors[i]);
    }
    //Interpolations
    for(int dim = 0; dim < dimensions.size(); dim--){
        std::vector<float> tempInterps{};
        for(int i = 0; i < dotProds.size(); i+=2){
            tempInterps.push_back(smoothstepinterp(dotProds[i], dotProds[i+1], distFromCorner[0][i*pow(2, dim)]));
        }
        dotProds = tempInterps;
    }
    //return
    return std::max(0.0, std::min((dotProds[0]+2.4)/4.8, 1.0));
}

std::vector<std::vector<int>> Perlin::combineArrays(std::vector<int> vec1, std::vector<int> vec2){
    //Prepare a vector of length 1 to exponentially grow
    std::vector<std::vector<int>> corners{};
    corners.resize(1);
    for(int i = 0; i < vec1.size(); i++){
        //This is the value that will be grown
        std::vector<std::vector<int>> newCorners{};

        //Copy and add current value from vec1 and vec2 to current value of corners
        for(int j = 0; j < corners.size(); j++){
            std::vector<int> temp1{corners[j]}, temp2{corners[j]};
            temp1.push_back(vec1[i]);
            temp2.push_back(vec2[i]);
            newCorners.push_back(temp1);
            newCorners.push_back(temp2);
        } 

        //replace corners for next iteration
        corners = newCorners;
    }
    return corners;
}

//This is a smoothstep function, makes transitions real smooth like
float Perlin::smoothstep(float start, float end, float val){
    return (val < start)? start : (val > end)? end: val*val*val*(10+val*(-15+val*6));
}

float Perlin::smoothstepinterp(float start, float end, float val){
    float perc = smoothstep(0, 1, val);
    return start*(1-perc)+end*(perc);
}

float Perlin::dot(std::vector<float> pos, std::vector<int> direction){
    float sum = 0;
    for(int i = 0; i < pos.size(); i++){
        sum += (float)direction[i]*pos[i];
    }
    return sum;
}
