#pragma once
#include <utility>

#include "menuBack.hpp"
#include "settings.hpp"

class NameChooser : public gfx::Scene {
    gfx::Button back_button, join_button;
    gfx::Sprite choose_name_title;
    gfx::TextInput name_input;
    BackgroundRect* menu_back;
    Settings* settings;
    std::string server_ip;
    bool can_join = true;
    void init() override;
    bool onKeyUp(gfx::Key key) override;
    void render() override;
    void stop() override;
public:
    NameChooser(BackgroundRect* menu_back, Settings* settings, std::string server_ip) : gfx::Scene("NameChooser"), menu_back(menu_back), settings(settings), server_ip(std::move(server_ip)) {}
};
