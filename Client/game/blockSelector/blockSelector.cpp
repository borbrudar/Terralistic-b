#include "blockSelector.hpp"

void BlockSelector::init() {
    select_rect.w = 2 * BLOCK_WIDTH;
    select_rect.h = 2 * BLOCK_WIDTH;
    select_rect.border_color = {255, 0, 0};
}

void BlockSelector::render() {
    if(!getKeyState(gfx::Key::MOUSE_LEFT) && is_left_button_pressed) {
        is_left_button_pressed = false;
        Packet packet;
        packet << ClientPacketType::STOPPED_BREAKING;
        networking->sendPacket(packet);
        prev_selected_x = blocks->getWidth();
        prev_selected_y = blocks->getHeight();
    }
    
    selected_block_x = (int)((getMouseX() + camera->getX() - gfx::getWindowWidth() / 2) / (BLOCK_WIDTH * 2));
    selected_block_y = (int)((getMouseY() + camera->getY() - gfx::getWindowHeight() / 2) / (BLOCK_WIDTH * 2));
    if((selected_block_x != prev_selected_x || selected_block_y != prev_selected_y) && is_left_button_pressed) {
        Packet packet;
        packet << ClientPacketType::STARTED_BREAKING << selected_block_x << selected_block_y;
        networking->sendPacket(packet);
        
        prev_selected_x = selected_block_x;
        prev_selected_y = selected_block_y;
    }
    
    select_rect.x = -camera->getX() + gfx::getWindowWidth() / 2 + selected_block_x * BLOCK_WIDTH * 2;
    select_rect.y = -camera->getY() + gfx::getWindowHeight() / 2 + selected_block_y * BLOCK_WIDTH * 2;
    select_rect.render();
}

bool BlockSelector::onKeyDown(gfx::Key key) {
    if(key == gfx::Key::MOUSE_RIGHT) {
        int starting_x = (player_handler->getMainPlayer()->getX()) / (BLOCK_WIDTH * 2);
        int starting_y = (player_handler->getMainPlayer()->getY()) / (BLOCK_WIDTH * 2);
        int ending_x = (player_handler->getMainPlayer()->getX() + PLAYER_WIDTH * 2 - 1) / (BLOCK_WIDTH * 2);
        int ending_y = (player_handler->getMainPlayer()->getY() + PLAYER_HEIGHT * 2 - 1) / (BLOCK_WIDTH * 2);

        if(selected_block_x < starting_x || selected_block_x > ending_x || selected_block_y < starting_y || selected_block_y > ending_y) {
            Packet packet;
            packet << ClientPacketType::RIGHT_CLICK << selected_block_x << selected_block_y;
            networking->sendPacket(packet);
        }
        
        return true;
    } else if(key == gfx::Key::MOUSE_LEFT) {
        is_left_button_pressed = true;
        return true;
    }
    return false;
}
