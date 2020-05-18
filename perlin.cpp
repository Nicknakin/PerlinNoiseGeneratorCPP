#include "perlin.h"

#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

Perlin::Perlin():Perlin{std::vector<int>{255,255}, 142857} {}
Perlin::Perlin(std::initializer_list<int> dims):Perlin{std::vector<int>{dims}} {}
Perlin::Perlin(std::vector<int> dimensions):Perlin{dimensions, 142857} {}
Perlin::Perlin(std::vector<int> dimensions, int seed) : dimensions{dimensions}, range{std::sqrt(dimensions.size())/2} {
    //Initialize an empty vector of length dimensions.size()
    dimensionLengths = std::vector<int> {};
    dimensionLengths.resize(dimensions.size());
    //Iterate through vector replacing each value with the product of all values before it
    for(unsigned int i = 0; i < dimensions.size(); i++) {
         dimensionLengths[i] = std::accumulate(dimensions.begin(), dimensions.begin()+i, 1, std::multiplies<int>());
    }

    //Base direction defaults to create a uniform 3^n-1 number of vectors pointing from the center out ward (will remove the {0,0,...0} value at end
    std::vector<int> BASES{0, 1, -1};
    //Initialized baseVectors as an empty vector to be expanded on
    baseVectors = std::vector<std::vector<int>> {std::vector<int>{}};

    //Iterate for the number of dimensions
    for(unsigned int i = 0; i < dimensions.size(); i++) {
        //Define a vector to fill with the expansion from baseVectors and BASES
        std::vector<std::vector<int>> expandedVec{};
        for(unsigned int j = 0; j < baseVectors.size(); j++) {
            //For each value in BASES do a push_back on the current value in baseVectors and push it to expandedVec (Turn every value in baseVectors in to three new values)
            for(unsigned int k = 0; k < 3; k++) {
                auto temp = baseVectors[j];
                temp.push_back(BASES[k]);
                expandedVec.push_back(temp);
            }
        }
        //Replace baseVectors with its expanded version for next iteration
        baseVectors = expandedVec;
    }

    //Remove the {0,0... 0} value which is always on top
    baseVectors.erase(baseVectors.begin());

    //Populate nodes
    std::srand(seed);
    int numNodes = 1;
    for(auto const &val : dimensions)
        numNodes *= val;
    nodes = std::vector<int> {};
    nodes.resize(numNodes);
    std::transform(nodes.begin(), nodes.end(), nodes.begin(), [&](auto _) -> int { return std::rand()%baseVectors.size();});

    //Calculate relative positions of corners around a unit "cube" from a point
    auto sub1 = std::vector<int> {}, sub2 = std::vector<int> {};
    sub1.resize(dimensions.size(), 0);
    sub2.resize(dimensions.size(), 1);
    subVector = combineArrays(sub1, sub2);
}

float Perlin::operator()(std::vector<float> pos) {
    //Get 0-1 version of position with whole number cut off
    std::vector<float> shortenedPos;
    shortenedPos.resize(pos.size());
    std::transform(pos.begin(), pos.end(), shortenedPos.begin(), [](const float& coord) -> float {return coord-floor(coord);});

    //Get Corners
    std::vector<int> topleft;
    topleft.resize(dimensions.size());
    std::transform(pos.begin(), pos.end(), dimensions.begin(), topleft.begin(), [&](float coord, int mod) -> int { return ((int) coord)%mod;});

    std::vector<std::vector<int>> cornerVectors{};
    cornerVectors.resize(subVector.size());
    std::transform(subVector.begin(), subVector.end(), cornerVectors.begin(), 
        [&](std::vector<int> corner) -> std::vector<int> {
            int cnt = 0;
            std::transform(corner.begin(), corner.end(), corner.begin(), 
                    [&] (int num) -> int {
                        int temp = (num + topleft[cnt])*dimensionLengths[cnt];
                        cnt++;
                        return temp;
                    });
            return baseVectors[nodes[std::accumulate(corner.begin(), corner.end(), 0)]];
        });
        

    //Dot Products
    std::vector<std::vector<float>> distFromCorner{};
    distFromCorner.resize(subVector.size(), shortenedPos);
    std::transform(distFromCorner.begin(), distFromCorner.end(), subVector.begin(), distFromCorner.begin(), [] (auto dist, auto sub) -> std::vector<float> {
                std::transform(dist.begin(), dist.end(), sub.begin(), dist.begin(), std::minus<float>()); 
                return dist;
            });

    std::vector<float> dotProds{};
    dotProds.resize(subVector.size());
    std::transform(distFromCorner.begin(), distFromCorner.end(), cornerVectors.begin(), dotProds.begin(), [&](auto a, auto b) {return dot(a,b);});

    //Interpolations
    int iter = shortenedPos.size()-1;
    while(dotProds.size() > 1) {
        std::vector<float> interpExpand{};
        for(unsigned int pairIndex = 0; pairIndex < dotProds.size(); pairIndex += 2) {
            interpExpand.push_back(smoothstepinterp(dotProds[pairIndex], dotProds[pairIndex+1], shortenedPos[iter]));
        }
        iter--;
        dotProds = interpExpand;
    }

    //return
    return dotProds[0]/range;
}

//Return a combination of the two arrays which grow exponentially
std::vector<std::vector<int>> Perlin::combineArrays(std::vector<int> vec1, std::vector<int> vec2) {
    //Prepare a vector of length 1 to exponentially grow
    std::vector<std::vector<int>> corners{};
    corners.resize(1);
    for(unsigned int i = 0; i < vec1.size(); i++) {
        //This is the value that will be grown
        std::vector<std::vector<int>> newCorners{};

        //Copy and add current value from vec1 and vec2 to current value of corners
        for(unsigned int j = 0; j < corners.size(); j++) {
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
float Perlin::smoothstep(float start, float end, float val) {
    return (val < start)? start : (val > end)? end: val*val*val*(10+val*(-15+val*6));
}

//Do a bi-interp from start to end at val% between the two
float Perlin::smoothstepinterp(float start, float end, float val) {
    float perc = smoothstep(0, 1, val);
    return start*(1-perc)+end*(perc);
}

//Simple dot product between floats and ints
float Perlin::dot(std::vector<float> pos, std::vector<int> direction) {
    float sum = 0;
    for(unsigned int i = 0; i < pos.size(); i++) {
        sum += (float)direction[i]*pos[i];
    }
    return sum;
}
