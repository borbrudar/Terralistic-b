#pragma once
#include "worldGenerator.hpp"
#include "serverEntities.hpp"
#include "serverItems.hpp"
#include "serverBlocks.hpp"
#include "serverLiquids.hpp"
#include "commands.hpp"

enum class ServerState {NEUTRAL, LOADING_WORLD, GENERATING_WORLD, RUNNING, STOPPING, STOPPED, CRASHED};

class Server {
    std::string world_path;
    ServerNetworking networking;
    ServerBlocks blocks;
    Biomes biomes;
    ServerLiquids liquids;
    WorldGenerator generator;
    ServerItems items;
    ServerPlayers players;
    ServerChat chat;
    Commands commands;
    ServerEntities entities;

    Recipes recipes;
    
    bool running = true;
    
    void saveWorld();
    void loadWorld();
    
    std::vector<ServerModule*> modules;
public:
    ServerState state = ServerState::NEUTRAL;
    
    Server(std::string resource_path, std::string world_path, int port);
    
    int seed;
    
    void start();
    void stop();
    
    void setPrivate(bool is_private);
    
    int getGeneratingTotal() const;
    int getGeneratingCurrent() const;
};
