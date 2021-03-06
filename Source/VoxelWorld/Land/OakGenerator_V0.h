/*================================================================
Filename: OakGenerator_V0.h
Date: 2018.1.26
Created by AirGuanZ
================================================================*/
#pragma once

#include "../Chunk/Chunk.h"
#include "../Chunk/ChunkLoader.h"
#include "LandGenerator_V0.h"

class OakGenerator_V0
{
public:
    using RandomEngine = typename LandGenerator_V0::RandomEngine;
    using Seed         = typename LandGenerator_V0::Seed;

    OakGenerator_V0(Seed seed);

    void Make(Chunk *ck) const;

private:
    float Random(Seed seedOffset, int blkX, int blkZ, float min, float max) const;

    void Try(Chunk *ck, int blkX, int blkY, int blkZ) const;
    
private:
    Seed seed_;
};
