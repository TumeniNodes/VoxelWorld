/*================================================================
Filename: LandGenerator_V0.h
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#pragma once

#include <random>
#include <vector>

#include "../Chunk/Chunk.h"

class LandGenerator_V0
{
public:
    using RandomEngine = std::default_random_engine;
    using Seed = RandomEngine::result_type;
    
    LandGenerator_V0(Seed seed);

    void GenerateLand(Chunk *ck);

private:
    int GetHeight(int blkX, int blkZ);
    float Random(Seed seedOffset, int blkX, int blkZ, float min, float max);

private:
    Seed seed_;
};
