#ifndef server_hpp
#define server_hpp

#include <string>
#include <utility>
#include "serverNetworking.hpp"
#include "worldGenerator.hpp"
#include "entities.hpp"
#include "items.hpp"
#include "serverPlayers.hpp"
#include "serverBlocks.hpp"
#include "serverLiquids.hpp"

enum class ServerState {NEUTRAL, STARTING, LOADING_WORLD, GENERATING_WORLD, RUNNING, STOPPING, STOPPED};

class Server {
    std::string world_path;
    ServerNetworkingManager networking_manager;
    ServerBlocks blocks;
    Biomes biomes;
    WorldGenerator generator;
    ServerLiquids liquids;
    Items items;
    ServerPlayers players;
    Entities entities;

    bool running = true;
    
    void saveWorld();
    void loadWorld();
public:
    ServerState state = ServerState::NEUTRAL;
    
    Server(std::string resource_path, std::string world_path);
    
    unsigned short seed;
    
    void start(unsigned short port);
    void stop();
    
    void setPrivate(bool is_private);
    
    unsigned int getGeneratingTotal() const;
    unsigned int getGeneratingCurrent() const;
};

#endif
