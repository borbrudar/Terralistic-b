#pragma once
#include <utility>
#include <vector>
#include <map>
#include "serverModule.hpp"
#include "events.hpp"
#include "graphics.hpp"

class WorldSaveEvent {};
class WorldLoadEvent {};

class WorldSaver : public ServerModule {
    void update(float frame_length) override;
    
    std::string world_path;
    std::map<std::string, std::vector<char>> sections;
    gfx::Timer timer;
public:
    explicit WorldSaver(std::string  world_path) : world_path(std::move(world_path)) {}
    
    void setSectionData(const std::string& name, const std::vector<char>& data);
    const std::vector<char>& getSectionData(const std::string& name);
    
    void load();
    void save();
    
    bool autosave_enabled = true;
    
    EventSender<WorldSaveEvent> world_save_event;
    EventSender<WorldLoadEvent> world_load_event;
};
