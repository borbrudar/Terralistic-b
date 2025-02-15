#include "serverLiquids.hpp"

bool cmpLiquidUpdates(LiquidUpdate& a, LiquidUpdate& b) {
    return a.time > b.time;
}

void ServerLiquids::onEvent(ServerConnectionWelcomeEvent &event) {
    Packet packet;
    packet << WelcomePacketType::LIQUIDS << toSerial();
    event.connection->send(packet);
}

void ServerLiquids::init() {
    networking->connection_welcome_event.addListener(this);
    liquid_change_event.addListener(this);
    blocks->block_change_event.addListener(this);
    world_saver->world_load_event.addListener(this);
    world_saver->world_save_event.addListener(this);
    
    liquid_update_queue = std::priority_queue<LiquidUpdate, std::vector<LiquidUpdate>, bool(*)(LiquidUpdate&, LiquidUpdate&)>(cmpLiquidUpdates);
}

void ServerLiquids::postInit() {
    liquid_schedules.resize(getWidth() * getHeight());
    
    for(int x = 0; x < getWidth(); x++)
        for(int y = 0; y < getHeight(); y++)
            setLiquidSchedule(x, y, false);
    
    for(int x = 1; x < getWidth() - 1; x++)
        for(int y = 1; y < getHeight() - 1; y++)
            updateLiquid(x, y);
}

void ServerLiquids::stop() {
    networking->connection_welcome_event.removeListener(this);
    liquid_change_event.removeListener(this);
    blocks->block_change_event.removeListener(this);
    world_saver->world_load_event.removeListener(this);
    world_saver->world_save_event.removeListener(this);
}

void ServerLiquids::setLiquidSchedule(int x, int y, bool value) {
    if(x < 0 || x >= getWidth() || y < 0 || y >= getHeight())
        throw Exception("Liquid schedule is accessed out of the bounds! (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    liquid_schedules[y * getWidth() + x] = value;
}

bool ServerLiquids::isLiquidScheduled(int x, int y) {
    if(x < 0 || x >= getWidth() || y < 0 || y >= getHeight())
        throw Exception("Liquid schedule is accessed out of the bounds! (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    return liquid_schedules[y * getWidth() + x];
}

void ServerLiquids::update(float frame_length) {
    int count = 0;
    while(count < 300 && !liquid_update_queue.empty() && liquid_update_queue.top().time < timer.getTimeElapsed()) {
        LiquidUpdate curr = liquid_update_queue.top();
        liquid_update_queue.pop();
        updateLiquid(curr.x, curr.y);
        setLiquidSchedule(curr.x, curr.y, false);
        count++;
    }
}

void ServerLiquids::scheduleLiquidUpdate(int x, int y) {
    if(!isLiquidScheduled(x, y) && getLiquidType(x, y) != &empty) {
        setLiquidSchedule(x, y, true);
        LiquidUpdate liquid_update;
        liquid_update.x = x;
        liquid_update.y = y;
        liquid_update.time = timer.getTimeElapsed() + getLiquidType(x, y)->flow_time;
        liquid_update_queue.push(liquid_update);
    }
}

void ServerLiquids::scheduleLiquidUpdateForNeighbours(int x, int y) {
    scheduleLiquidUpdate(x, y);
    if(x > 0)
        scheduleLiquidUpdate(x - 1, y);
    if(y > 0)
        scheduleLiquidUpdate(x, y - 1);
    if(x < getWidth() - 1)
        scheduleLiquidUpdate(x + 1, y);
    if(y < getHeight() - 1)
        scheduleLiquidUpdate(x, y + 1);
}

void ServerLiquids::onEvent(LiquidChangeEvent& event) {
    scheduleLiquidUpdateForNeighbours(event.x, event.y);
    
    Packet packet;
    packet << ServerPacketType::LIQUID << event.x << event.y << getLiquidType(event.x, event.y)->id << getLiquidLevel(event.x, event.y);
    networking->sendToEveryone(packet);
}

void ServerLiquids::onEvent(BlockChangeEvent& event) {
    scheduleLiquidUpdateForNeighbours(event.x, event.y);
}

void ServerLiquids::onEvent(WorldSaveEvent &event) {
    world_saver->setSectionData("liquids", toSerial());
}

void ServerLiquids::onEvent(WorldLoadEvent &event) {
    fromSerial(world_saver->getSectionData("liquids"));
}
