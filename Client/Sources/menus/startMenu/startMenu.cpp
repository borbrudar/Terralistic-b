//
//  startMenu.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 06/07/2020.
//

#include "startMenu.hpp"
#include "worldSelector.hpp"
#include "multiplayerSelector.hpp"

// this is the main menu, which you see on the start of the app

void startMenu::init() {
    singleplayer_button.scale = 3;
    singleplayer_button.setTexture(gfx::renderText("Singleplayer", {255, 255, 255}));
    singleplayer_button.y = short(-singleplayer_button.getTranslatedRect().h - 5);
    singleplayer_button.orientation = gfx::CENTER;

    multiplayer_button.scale = 3;
    multiplayer_button.setTexture(gfx::renderText("Multiplayer", {255, 255, 255}));
    multiplayer_button.orientation = gfx::CENTER;

    exit_button.scale = 3;
    exit_button.setTexture(gfx::renderText("Exit", {255, 255, 255}));
    exit_button.y = short(exit_button.getTranslatedRect().h + 5);
    exit_button.orientation = gfx::CENTER;
    
    title.setTexture(gfx::renderText("Terralistic", {255, 255, 255}));
    title.scale = 4;
    title.orientation = gfx::TOP;
    title.y = 40;
    
    background.setTexture(gfx::loadImageFile("texturePack/misc/background.png"));
    
    back_rect.c = {0, 0, 0};
    back_rect.orientation = gfx::CENTER;
    back_rect.w = singleplayer_button.getWidth() + 100;
#ifdef DEVELOPER_MODE
    debug_title.setTexture(gfx::renderText("DEBUG MODE", {100, 100, 100}));
    debug_title.orientation = gfx::TOP;
    debug_title.scale = 2;
    debug_title.y = 10;
#endif
    
    version.setTexture(gfx::renderText(
#include "version.hpp"
                                       , {100, 100, 100}));
    version.orientation = gfx::BOTTOM;
    version.scale = 2;
    version.y = -5;
}

void startMenu::onKeyDown(gfx::key key) {
    if(key == gfx::KEY_MOUSE_LEFT) {
        if(exit_button.isHovered())
            gfx::returnFromScene();
        else if(singleplayer_button.isHovered())
            worldSelector().run();
        else if(multiplayer_button.isHovered())
            multiplayerSelector().run();
    }
}

void startMenu::render() {
    float scale = (float)gfx::getWindowHeight() / (float)background.getTextureHeight();
    int pos = gfx::getTicks() / 30 % int(background.getTextureWidth() * scale);
    background.render(scale, pos, 0);
    background.render(scale, pos - background.getTextureWidth() * scale, 0);
    back_rect.h = gfx::getWindowHeight();
    back_rect.render();
    title.render();
#ifdef DEVELOPER_MODE
    debug_title.render();
#endif
    version.render();
    singleplayer_button.render();
    multiplayer_button.render();
    exit_button.render();
}
