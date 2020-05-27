#include "perlin.h"
#include <vector>
#include <time.h>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>

Perlin::Perlin():Perlin{std::vector<int>{255,255}, 142857} {}
Perlin::Perlin(std::initializer_list<int> dims):Perlin{std::vector<int>{dims}} {}
Perlin::Perlin(std::vector<int> dimensions) : Perlin{dimensions, 1, 142857} {}
Perlin::Perlin(std::vector<int> dimensions, int octaves) : Perlin{dimensions, octaves, 142857} {} 
Perlin::Perlin(std::vector<int> dimensions, int octaves, int seed) : dimensions{dimensions}, octaves{octaves}, range{sqrt(dimensions.size())/2}{
    //Initialize an empty vector of length dimensions.size()
    dimensionLengths = std::vector<int> {};
    dimensionLengths.resize(dimensions.size());
    //Iterate through vector replacing each value with the product of all values before it
    for(unsigned int i = 0; i < dimensions.size(); i++) {
        dimensionLengths[i] = std::accumulate(dimensions.begin(), dimensions.begin()+i, 1, std::multiplies<int>());
    }

    //Base direction defaults to create a uniform 3^n-1 number of vectors pointing from the center out ward (will remove the {0,0,...0} value at end
    std::vector<double> BASES{0.0, 1.0, -1.0};
    //Initialized baseVectors as an empty vector to be expanded on
    baseVectors = std::vector<std::vector<double>> {std::vector<double>{}};

    //Iterate for the number of dimensions
    for(unsigned int i = 0; i < dimensions.size(); i++) {
        //Define a vector to fill with the expansion from baseVectors and BASES
        std::vector<std::vector<double>> expandedVec{};
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

    std::transform(baseVectors.begin(), baseVectors.end(), baseVectors.begin(), [] (auto vec) {
            const auto mag = sqrt(std::inner_product(vec.begin(), vec.end(), vec.begin(), 0));
            std::transform(vec.begin(), vec.end(), vec.begin(), [&] (auto num) { return num/=mag; });
            return vec;
    });

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

    octAdjust = 0;
    reductionBase = 2;
    divisors.resize(octaves);
    for(double i = 0; i < octaves; i++){
        divisors[i] = std::pow(reductionBase, i);
    }
    octAdjust = (double) ((1 << octaves) - 1 ) / (double) ( 1 << (octaves-1));
}

double Perlin::operator()(std::vector<double> pos) {
    std::vector<std::vector<double>> octs;
    octs.resize(octaves);
    for(auto &val : octs)
        val = pos;
    std::transform(octs.begin(), octs.end(), divisors.begin(), octs.begin(), [&](auto pos, auto b) {
            std::transform(pos.begin(), pos.end(), pos.begin(), [&](auto num) {
                    return num*b;
                    });
            return pos;
            });
    std::vector<double> octVals;
    octVals.resize(octs.size());
    std::transform(octs.begin(), octs.end(), divisors.begin(), octVals.begin(), [&] (auto a, auto b) {
            return noise(a)/b;
            });
    double temp = 0;
    for(const auto val : octVals) temp += val;
    return temp;
}

double Perlin::noise(std::vector<double> pos) {
    pos.resize(dimensions.size(), 0.5);
    //Get 0-1 version of position with whole number cut off
    std::vector<double> shortenedPos;
    shortenedPos.resize(pos.size());
    std::transform(pos.begin(), pos.end(), shortenedPos.begin(), [](const double& coord) -> double {return coord-floor(coord);});

    //Get Corners
    std::vector<int> topleft;
    topleft.resize(dimensions.size());
    std::transform(pos.begin(), pos.end(), topleft.begin(), [&](double coord) -> int { return ((int) coord);});

    std::vector<std::vector<double>> cornerVectors{};
    cornerVectors.resize(subVector.size());
    //subVector (base) | topLeft (origin) | dimensionLengths (for indeces) | cornerVectors (output)
    std::transform(subVector.begin(), subVector.end(), cornerVectors.begin(), 
            [&](std::vector<int> corner) {
            int cnt = 0;
            std::transform(corner.begin(), corner.end(), corner.begin(), 
                    [&] (int num) -> int {
                    int offset = (num + topleft[cnt]);
                    offset %= dimensions[cnt];
                    int temp = offset*dimensionLengths[cnt];
                    cnt++;
                    return temp;
                    });
            return baseVectors[nodes[std::accumulate(corner.begin(), corner.end(), 0)]];
            });


    //Dot Products
    std::vector<std::vector<double>> distFromCorner{};
    distFromCorner.resize(subVector.size(), shortenedPos);
    std::transform(distFromCorner.begin(), distFromCorner.end(), subVector.begin(), distFromCorner.begin(), [] (auto dist, auto sub) -> std::vector<double> {
            std::transform(dist.begin(), dist.end(), sub.begin(), dist.begin(), std::minus<double>()); 
            return dist;
            });

    std::vector<double> dotProds{};
    dotProds.resize(subVector.size());
    std::transform(distFromCorner.begin(), distFromCorner.end(), cornerVectors.begin(), dotProds.begin(), [&](auto a, auto b) {return dot(a,b);});

    //Interpolations
    int iter = shortenedPos.size()-1;
    while(dotProds.size() > 1) {
        std::vector<double> interpExpand{};
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
double Perlin::smoothstep(double start, double end, double val) {
    return (val < start)? start : (val > end)? end: val*val*val*(10+val*(-15+val*6));
}

//Do a bi-interp from start to end at val% between the two
double Perlin::smoothstepinterp(double start, double end, double val) {
    double perc = smoothstep(0, 1, val);
    return start*(1-perc)+end*(perc);
}

//Simple dot product between doubles and ints
double Perlin::dot(std::vector<double> pos, std::vector<double> direction) {
    double sum = 0;
    for(unsigned int i = 0; i < pos.size(); i++) {
        sum += (double)direction[i]*pos[i];
    }
    return sum;
}
