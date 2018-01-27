/*================================================================
Filename: LandGenerator_V0.cpp
Date: 2018.1.21
Created by AirGuanZ
================================================================*/
#include "../Chunk/Chunk.h"
#include "LandGenerator_V0.h"
#include "OakGenerator_V0.h"

LandGenerator_V0::LandGenerator_V0(Seed seed)
    : seed_(seed)
{

}

void LandGenerator_V0::GenerateLand(Chunk *ck, std::vector<IntVector3> &lightUpdates)
{
    assert(ck != nullptr);

    Chunk::BlockData &data = ck->GetBlockData();
    Chunk::HeightMap &heightMap = ck->GetHeightMap();
    IntVectorXZ ckPos = ck->GetPosition();
    int xBase = ChunkXZ_To_BlockXZ(ckPos.x);
    int zBase = ChunkXZ_To_BlockXZ(ckPos.z);
    for(int x = 0; x != CHUNK_SECTION_SIZE; ++x)
    {
        for(int z = 0; z != CHUNK_SECTION_SIZE; ++z)
        {
            int h = GetHeight(x + xBase, z + zBase);

            data[x][0][z].type = BlockType::Bedrock;
            SetLight(data[x][0][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                                    LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN);

            for(int y = 1; y != h - 2; ++y)
            {
                data[x][y][z].type = BlockType::Stone;
                SetLight(data[x][y][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                    LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN);
            }

            for(int y = h - 2; y != h; ++y)
            {
                data[x][y][z].type = BlockType::Dirt;
                SetLight(data[x][y][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                    LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN);
            }

            constexpr int WATER_LEVEL = 40;
            if(h < WATER_LEVEL)
            {
                for(int y = h; y <= WATER_LEVEL; ++y)
                {
                    data[x][y][z].type = BlockType::Water;
                    SetLight(data[x][y][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                        (std::max)(0, LIGHT_COMPONENT_MAX - 2 * (WATER_LEVEL - y + 1)));
                }
                h = WATER_LEVEL;
            }
            else
            {
                data[x][h][z].type = BlockType::GrassBox;
                SetLight(data[x][h][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                    LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN);

                float gfv = Random(1, x + xBase, z + zBase, 0.0f, 1.0f);
                if(gfv < 0.02f)
                {
                    data[x][h + 1][z].type = BlockType::Grass;
                    SetLight(data[x][h + 1][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                        LIGHT_COMPONENT_MAX - 1);
                    ++h;
                }
                else if(gfv < 0.04f)
                {
                    data[x][h + 1][z].type = BlockType::Flower;
                    SetLight(data[x][h + 1][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                        LIGHT_COMPONENT_MAX - 1);
                    ++h;
                }

            }

            for(int y = h + 1; y != CHUNK_MAX_HEIGHT; ++y)
            {
                data[x][y][z].type = BlockType::Air;
                SetLight(data[x][y][z], LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN, LIGHT_COMPONENT_MIN,
                    LIGHT_COMPONENT_MAX);
            }

            heightMap[x][z] = h;
        }
    }

    //������
    OakGenerator_V0(seed_).Make(ck, lightUpdates);
}

int LandGenerator_V0::GetHeight(int x, int z)
{
    constexpr int LEVEL_NUM = 5;
    constexpr int MAX_GRID_SIZE = 128;
    constexpr float MAX_LEVEL_HEIGHT = 30.0f;

    int gridSize = MAX_GRID_SIZE;
    float levelHeight = MAX_LEVEL_HEIGHT;
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    auto randXZ = [&](int x, int z, int level)
    {
        return dis(RandomEngine(seed_ * x + z + level * level));
    };

    auto lerp = [](float a, float b, float t)
    {
        return a + (b - a) * (3 * t * t - 2 * t * t * t);
    };

    float result = 10.0f;
    for(int level = 0; level != LEVEL_NUM; ++level)
    {
        int gridX = (x + 0x40000000) / gridSize - 0x40000000 / gridSize;
        int gridZ = (z + 0x40000000) / gridSize - 0x40000000 / gridSize;

        float xzH   = randXZ(gridX, gridZ, level);
        float x1zH  = randXZ(gridX + 1, gridZ, level);
        float xz1H  = randXZ(gridX, gridZ + 1, level);
        float x1z1H = randXZ(gridX + 1, gridZ + 1, level);
        float tX    = static_cast<float>(x - gridX * gridSize) / gridSize;
        float tZ    = static_cast<float>(z - gridZ * gridSize) / gridSize;

        result += levelHeight * lerp(lerp(xzH, x1zH, tX), lerp(xz1H, x1z1H, tX), tZ);
        gridSize /= 2;
        levelHeight *= 0.7f;
    }

    return static_cast<int>(result);
}

float LandGenerator_V0::Random(Seed seedOffset, int blkX, int blkZ, float min, float max)
{
    return std::uniform_real_distribution<float>(min, max)(
        RandomEngine((seed_ + seedOffset) * blkX + blkZ));
}
