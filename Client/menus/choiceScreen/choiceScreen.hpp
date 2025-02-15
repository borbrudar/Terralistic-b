#pragma once
#include "menuBack.hpp"

class ChoiceScreenButton {
public:
    std::string option;
    gfx::Button gfx_button;
};

class ChoiceScreen : public gfx::Scene {
    gfx::Sprite question_sprite;
    std::string question, *result;
    std::vector<ChoiceScreenButton*> buttons;
    BackgroundRect* menu_back;
    
    void init() override;
    bool onKeyUp(gfx::Key key) override;
    void render() override;
public:
    ChoiceScreen(BackgroundRect* menu_back, std::string question, const std::vector<std::string>& options, std::string* result = nullptr);
};
