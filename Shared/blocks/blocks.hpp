#pragma once
#include "events.hpp"
#include "graphics.hpp"
#include "exception.hpp"

#define BLOCK_WIDTH 8
#define UNBREAKABLE -1

class BlockChangeEvent {
public:
    BlockChangeEvent(int x, int y) : x(x), y(y) {}
    int x, y;
};

class BlockBreakEvent {
public:
    BlockBreakEvent(int x, int y) : x(x), y(y) {}
    int x, y;
};

class BlockStartedBreakingEvent {
public:
    BlockStartedBreakingEvent(int x, int y) : x(x), y(y) {}
    int x, y;
};

class BlockStoppedBreakingEvent {
public:
    BlockStoppedBreakingEvent(int x, int y) : x(x), y(y) {}
    int x, y;
};

class BlockType {
public:
    BlockType() = default;
    BlockType(std::string name, bool ghost, bool transparent, int break_time, std::vector<BlockType*> connects_to, gfx::Color color);
    
    bool ghost, transparent;
    std::string name;
    std::vector<BlockType*> connects_to;
    int break_time;
    gfx::Color color;
    unsigned char id;
};

namespace BlockTypes {
    inline BlockType air("air", /*ghost*/true, /*transparent*/true, /*break_time*/UNBREAKABLE, /*connects_to*/ {}, /*color*/{0, 0, 0, 0});
};

class Blocks {
    class Block {
    public:
        unsigned char id = BlockTypes::air.id;
    };
    
    class BreakingBlock {
    public:
        int break_progress = 0;
        bool is_breaking = true;
        int x, y;
    };
    
    Block *blocks = nullptr;
    int width, height;
    
    std::vector<BreakingBlock> breaking_blocks;
    std::vector<BlockType*> block_types;
    
    Block* getBlock(int x, int y);
public:
    Blocks();
    void create(int width, int height);

    BlockType* getBlockType(int x, int y);
    void setBlockType(int x, int y, BlockType* type);
    void setBlockTypeSilently(int x, int y, BlockType* type);
    
    int getBreakProgress(int x, int y);
    unsigned char getBreakStage(int x, int y);
    void startBreakingBlock(int x, int y);
    void stopBreakingBlock(int x, int y);
    void updateBreakingBlocks(int frame_length);
    
    void breakBlock(int x, int y);
    
    int getWidth() const;
    int getHeight() const;
    
    void serialize(std::vector<char>& serial);
    char* loadFromSerial(char* iter);
    
    void registerNewBlockType(BlockType* block_type);
    BlockType* getBlockTypeById(unsigned char block_id);
    BlockType* getBlockTypeByName(const std::string& name);
    unsigned char getNumBlockTypes();
    
    EventSender<BlockChangeEvent> block_change_event;
    EventSender<BlockBreakEvent> block_break_event;
    EventSender<BlockStartedBreakingEvent> block_started_breaking_event;
    EventSender<BlockStoppedBreakingEvent> block_stopped_breaking_event;
    
    ~Blocks();
};

class BlockOutOfBoundsException : public Exception {
public:
    BlockOutOfBoundsException(int x, int y) : Exception("Block is accessed out of the bounds! (" + std::to_string(x) + ", " + std::to_string(y) + ")") {}
};
