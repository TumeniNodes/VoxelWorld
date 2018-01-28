/*================================================================
Filename: Chunk.h
Date: 2018.1.18
Created by AirGuanZ
================================================================*/
#ifndef VW_CHUNK_H
#define VW_CHUNK_H

#include "../Block/Block.h"
#include "../Model/BasicModel.h"
#include "../Renderer/BasicRenderer.h"
#include "../Renderer/CarveRenderer.h"
#include "../Renderer/LiquidRenderer.h"
#include "../Renderer/RenderQueue.h"
#include "../Utility/Math.h"
#include "../Utility/Uncopiable.h"

//2^4，这里修改的话XYZ也要修改
constexpr int CHUNK_SECTION_SIZE = 16;

//2^3，这里修改的话XYZ也要修改
constexpr int CHUNK_SECTION_NUM  = 8;

//2^7，这里修改的话XYZ也要修改
constexpr int CHUNK_MAX_HEIGHT   = CHUNK_SECTION_SIZE * CHUNK_SECTION_NUM;

constexpr int CHUNK_BLOCK_NUM = CHUNK_SECTION_SIZE * CHUNK_SECTION_SIZE * CHUNK_MAX_HEIGHT;

constexpr int BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET = 0x40000000;

inline int BlockXZ_To_ChunkXZ(int blk)
{
    return (blk + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET) / CHUNK_SECTION_SIZE
        - BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET / CHUNK_SECTION_SIZE;
}

inline int BlockXZ_To_BlockXZInChunk(int blk)
{
    return (blk + BLOCK_POSITION_CONVENTION_POSITIVE_OFFSET) % CHUNK_SECTION_SIZE;
}

inline int BlockY_To_ChunkSectionIndex(int blk)
{
    return blk / CHUNK_SECTION_SIZE;
}

inline int BlockY_To_BlockYInChunkSection(int blk)
{
    return blk % CHUNK_SECTION_SIZE;
}

inline int ChunkXZ_To_BlockXZ(int ck)
{
    return ck * CHUNK_SECTION_SIZE;
}

inline int ChunkSectionIndex_To_BlockY(int cks)
{
    return cks * CHUNK_SECTION_SIZE;
}

inline int Camera_To_Block(float cam)
{
    //IMPROVE
    return static_cast<int>(std::floor(cam));
}

inline IntVectorXZ BlockXZ_To_ChunkXZ(const IntVectorXZ &in)
{
    return { BlockXZ_To_ChunkXZ(in.x), BlockXZ_To_ChunkXZ(in.z) };
}

inline IntVectorXZ BlockXZ_To_BlockXZInChunk(const IntVectorXZ &in)
{
    return { BlockXZ_To_BlockXZInChunk(in.x), BlockXZ_To_BlockXZInChunk(in.z) };
}

class ChunkManager;

struct ChunkSectionModels
{
    BasicModel basic[BASIC_RENDERER_TEXTURE_NUM];
    CarveModel carve[CARVE_RENDERER_TEXTURE_NUM];
    LiquidModel liquid[LIQUID_RENDERER_TEXTURE_NUM];
};

struct ChunkSectionRenderQueue
{
    RenderQueue basic[BASIC_RENDERER_TEXTURE_NUM];
    RenderQueue carve[CARVE_RENDERER_TEXTURE_NUM];
    RenderQueue liquid[LIQUID_RENDERER_TEXTURE_NUM];
};

class Chunk : public Uncopiable
{
public:
    Chunk(ChunkManager *ckMgr, const IntVectorXZ &ckPos);
    ~Chunk(void);

    static int XYZ(int x, int y, int z)
    {
        assert(0 <= x && x < CHUNK_SECTION_SIZE);
        assert(0 <= z && z < CHUNK_SECTION_SIZE);
        assert(0 <= y && y < CHUNK_MAX_HEIGHT);
        return (x << 11) | (z << 7) | y;
    }

    static int XZ(int x, int z)
    {
        assert(0 <= x && x < CHUNK_SECTION_SIZE);
        assert(0 <= z && z < CHUNK_SECTION_SIZE);
        return (x << 4) | z;
    }

    IntVectorXZ GetPosition(void) const
    {
        return ckPos_;
    }

    int GetXPosBase(void) const
    {
        return ChunkXZ_To_BlockXZ(ckPos_.x);
    }

    int GetZPosBase(void) const
    {
        return ChunkXZ_To_BlockXZ(ckPos_.z);
    }

    using BlockTypeData  = BlockType[CHUNK_BLOCK_NUM];
    using BlockLightData = BlockLight[CHUNK_BLOCK_NUM];
    using HeightMap = int[CHUNK_SECTION_SIZE * CHUNK_SECTION_SIZE];

    BlockTypeData blocks;
    BlockLightData lights;

    HeightMap heightMap;

    Block GetBlock(int x, int y, int z)
    {
        assert(0 <= x && x < CHUNK_SECTION_SIZE);
        assert(0 <= z && z < CHUNK_SECTION_SIZE);
        assert(0 <= y && y < CHUNK_MAX_HEIGHT);

        int idx = XYZ(x, y, z);
        return { blocks[idx], lights[idx] };
    }

    void SetModels(int section, ChunkSectionModels *models)
    {
        assert(0 <= section && section < CHUNK_SECTION_NUM);
        Helper::SafeDeleteObjects(models_[section]);
        models_[section] = models;
    }

    ChunkSectionModels *GetModels(int section)
    {
        assert(0 <= section && section < CHUNK_SECTION_NUM);
        return models_[section];
    }

    void Render(ChunkSectionRenderQueue *renderQueue);

private:
    ChunkManager *ckMgr_;
    IntVectorXZ ckPos_;

    //下标的使用：[x][y][z]
    ChunkSectionModels *models_[CHUNK_SECTION_NUM];
};

#endif //VW_CHUNK_H
