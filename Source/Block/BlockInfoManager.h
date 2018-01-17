/*================================================================
Filename: BlockInfoManager.h
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#ifndef VW_BLOCK_INFO_MANAGER_H
#define VW_BLOCK_INFO_MANAGER_H

#include "../Utility/Singleton.h"
#include "BlockInfo.h"

class BlockInfoManager : public Singleton<BlockInfoManager>
{
public:
    BlockInfoManager::BlockInfoManager(void);

    const BlockInfo &GetBlockInfo(BlockType type) const;

    bool IsFaceVisible(BlockType dst, BlockType neighbor) const;

private:
    std::vector<BlockInfo> info_;
};

#endif //VW_BLOCK_INFO_MANAGER_H