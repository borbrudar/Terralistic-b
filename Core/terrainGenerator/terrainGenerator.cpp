//
//  terrainGenerator.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 25/06/2020.
//

#include "core.hpp"

#include "simplex-noise.hpp"

// terrain generation parameters

// TERRAIN
#define TERRAIN_VERTICAL_MULTIPLIER 140
#define TERRAIN_HORIZONTAL_DIVIDER 4
#define TERRAIN_HORIZONT ((float)blockEngine::world_height / 2 - 50)

// CAVES

#define CAVE_START 0.15
#define CAVE_LENGTH 0.3

#define STONE_START 0.69
#define STONE_LENGTH 1

//xPeriod and yPeriod together define the angle of the lines
//xPeriod and yPeriod both 0 ==> it becomes a normal clouds or turbulence pattern
//#define X_PERIOD 0.5 //defines repetition of marble lines in x direction
//#define Y_PERIOD 1.0 //defines repetition of marble lines in y direction
//turbPower = 0 ==> it becomes a normal sine pattern
//#define TURB_POWER 5.0 //makes twists
#define TURB_SIZE 64.0 //initial size of the turbulence

void stackDirt(unsigned int x, unsigned int height);

void generateSurface(std::mt19937& engine);
void generateCaves(std::mt19937& engine);
void generateStone(std::mt19937& engine);
void generateTrees(std::mt19937& engine);

static unsigned int highest_height = 0;
static unsigned short* heights;

#define LOADING_NEXT terrainGenerator::loading_current++;

int terrainGenerator::generateTerrainDaemon(unsigned int seed) {
    std::mt19937 engine(seed);
    generateSurface(engine);
    generateCaves(engine);
    generateStone(engine);
    generateTrees(engine);
    for(int y = 0; y < (blockEngine::world_height >> 4); y++)
        for(int x = 0; x < (blockEngine::world_width >> 4); x++)
            blockEngine::getChunkState(x, y) = blockEngine::loaded;
    LOADING_NEXT
    return 0;
}

void stackDirt(unsigned int x, unsigned int height) {
    for(auto y = (unsigned int)(blockEngine::world_height - 1); y > blockEngine::world_height - height - 1; y--)
        blockEngine::getBlock((unsigned short)x, (unsigned short)y).block_id = blockEngine::DIRT;
    blockEngine::getBlock((unsigned short)x,
                          (unsigned short)(blockEngine::world_height - height - 1)).block_id = blockEngine::GRASS_BLOCK;
}

double turbulence(double x, double y, double size, PerlinNoise noise) {

//double turbulence(double x, double y, double size, double x_period, double y_period, double turb_power, PerlinNoise noise) {
    double value = 0.0, initialSize = size;

    while(size >= 2) {
      value += noise.noise(x / size, y / size) * size;
      size /= 2.0;
    }
    
    return value / initialSize;
    
    //double xy_value = x * x_period / blockEngine::world_width + y * y_period / highest_height + turb_power * value / initialSize / 2.0;
    //return fabs(sin(xy_value * M_PI));
}

void generateSurface(std::mt19937& engine) {
    PerlinNoise noise(engine());
    
    heights = new unsigned short[blockEngine::world_width];
    
    // generate terrain
    for(unsigned int x = 0; x < blockEngine::world_width; x++) {
        // apply multiple layers of perlin noise
        auto height = (unsigned int)(TERRAIN_HORIZONT + TERRAIN_VERTICAL_MULTIPLIER * turbulence((double) x / TERRAIN_HORIZONTAL_DIVIDER, 0.8, TURB_SIZE, noise));
        
        heights[x] = height;
        if(height > highest_height)
            highest_height = height;
    }
    LOADING_NEXT

    // apply terrain to world
    for(unsigned int x = 0; x < blockEngine::world_width; x++) {
        stackDirt(x, heights[x]);
        if(!(engine() % 7)) // generate stones
            blockEngine::getBlock((unsigned short)x,
                                  (unsigned short)(blockEngine::world_height - heights[x] - 2)).block_id = blockEngine::STONE;
    }
    LOADING_NEXT
    
    blockEngine::getBlock(0, 0).block_id = blockEngine::DIRT;
}

void generateCaves(std::mt19937& engine) {
    osn_context* ctx;
    open_simplex_noise(engine(), &ctx);
    
    for(unsigned int y = 0; y < highest_height; y++)
        for(unsigned int x = 0; x < blockEngine::world_width; x++)
            if(open_simplex_noise2(ctx, (double)x / 10, (double)y / 10) > CAVE_START + CAVE_LENGTH - (double)y / highest_height * CAVE_LENGTH)
                blockEngine::getBlock((unsigned short)x, (unsigned short)(y + (blockEngine::world_height - highest_height))).block_id = blockEngine::AIR;
    open_simplex_noise_free(ctx);
    LOADING_NEXT
}

void generateStone(std::mt19937& engine) {
    PerlinNoise noise(engine());
    for(unsigned int y = blockEngine::world_height - highest_height; y < blockEngine::world_height; y++)
        for(unsigned int x = 0; x < blockEngine::world_width; x++)
            if(blockEngine::getBlock((unsigned short)x, (unsigned short)y).block_id && turbulence(x * 5, y * 5, TURB_SIZE, noise) > STONE_START + STONE_LENGTH - (double)y / highest_height * STONE_LENGTH)
                blockEngine::getBlock((unsigned short)x, (unsigned short)y).block_id = blockEngine::STONE_BLOCK;
    LOADING_NEXT
}

void generateTrees(std::mt19937& engine) {
    unsigned short x = 0;
    while(true) {
        x += engine() % 4 + 6;
        if(x >= blockEngine::world_width - 1)
            break;
        if(blockEngine::getBlock(x, blockEngine::world_height - heights[x] - 1).block_id != blockEngine::GRASS_BLOCK)
            continue;
        unsigned short height = engine() % 5 + 10;
        unsigned short y;
        for(y = blockEngine::world_height - heights[x] - 2; y > blockEngine::world_height - heights[x] - height; y--)
            blockEngine::getBlock(x, y).block_id = blockEngine::WOOD;
        
        for(unsigned short y_leave = y; y_leave < y + 5; y_leave++)
            for(unsigned short x_leave = x - 2; x_leave <= x + 2; x_leave++)
                if((x_leave != x - 2 || y_leave != y) && (x_leave != x + 2 || y_leave != y) && (x_leave != x - 2 || y_leave != y + 4) && (x_leave != x + 2 || y_leave != y + 4))
                    blockEngine::getBlock(x_leave, y_leave).block_id = blockEngine::LEAVES;
        
        for(unsigned short y2 = blockEngine::world_height - heights[x] - engine() % 4 - 4; y2 > blockEngine::world_height - heights[x] - height + 5; y2 -= engine() % 4 + 2) {
            blockEngine::getBlock(x - 1, y2).block_id = blockEngine::WOOD;
            blockEngine::getBlock(x - 2, y2).block_id = blockEngine::LEAVES;
        }
        
        for(unsigned short y2 = blockEngine::world_height - heights[x] - engine() % 4 - 4; y2 > blockEngine::world_height - heights[x] - height + 5; y2 -= engine() % 4 + 2) {
            blockEngine::getBlock(x + 1, y2).block_id = blockEngine::WOOD;
            blockEngine::getBlock(x + 2, y2).block_id = blockEngine::LEAVES;
        }
        
        if(blockEngine::getBlock(x - 1, blockEngine::world_height - heights[x] - 1).block_id == blockEngine::GRASS_BLOCK && blockEngine::getBlock(x - 2, blockEngine::world_height - heights[x] - 2).block_id == blockEngine::AIR)
            blockEngine::getBlock(x - 1, blockEngine::world_height - heights[x] - 2).block_id = blockEngine::WOOD;
        
        if(blockEngine::getBlock(x + 1, blockEngine::world_height - heights[x] - 1).block_id == blockEngine::GRASS_BLOCK && blockEngine::getBlock(x + 2, blockEngine::world_height - heights[x] - 2).block_id == blockEngine::AIR)
            blockEngine::getBlock(x + 1, blockEngine::world_height - heights[x] - 2).block_id = blockEngine::WOOD;
    }
    LOADING_NEXT
}
