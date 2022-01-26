#include <set>
#include "chat.hpp"

static const std::set<char> allowed_chars = {'!', ':', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', '"', '|', '~', '<', '>', '?', '-', '=', ',', '.', '/', '[', ']', ';', '\'', '\\', '`', ' '};

void Chat::init() {
    networking->packet_event.addListener(this);
    chat_box.scale = 2;
    chat_box.setText("");
    chat_box.orientation = gfx::BOTTOM_LEFT;
    chat_box.y = -SPACING / 2;
    chat_box.x = SPACING / 2;
    chat_box.textProcessing = [](char c, int length) {
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || allowed_chars.find(c) != allowed_chars.end())
            return c;
        return '\0';
    };
    
    chat_box.def_color.a = TRANSPARENCY;
    chat_box.setBlurIntensity(BLUR);
    chat_box.setBorderColor(BORDER_COLOR);
    chat_box.width = 100;
    
    text_inputs = {&chat_box};
}

void Chat::update(float frame_length) {
    int target_width = chat_box.active ? 300 : 100;
    chat_box.width += (target_width - (int)chat_box.width) / 3;
    
    for(int i = 0; i < chat_lines.size(); i++)
        chat_lines[i]->text_sprite.y += (chat_lines[i]->y_to_be - chat_lines[i]->text_sprite.y) / 2;
}

void Chat::render() {
    chat_box.render(getMouseX(), getMouseY());
    
    for(int i = 0; i < chat_lines.size(); i++) {
        if(!chat_lines[i]->text.empty()) {
            chat_lines[i]->text_sprite.loadFromText(chat_lines[i]->text);
            chat_lines[i]->text_sprite.scale = 2;
            chat_lines[i]->text_sprite.y = chat_box.y;
            chat_lines[i]->text_sprite.x = SPACING / 2;
            chat_lines[i]->text_sprite.orientation = gfx::BOTTOM_LEFT;
            chat_lines[i]->y_to_be = chat_box.y - chat_box.getHeight();
            chat_lines[i]->text.clear();
            chat_lines[i]->text.shrink_to_fit();
            
            for(int i2 = 0; i2 < chat_lines.size(); i2++)
                if(chat_lines[i2] != chat_lines[i])
                    chat_lines[i2]->y_to_be -= chat_lines[i]->text_sprite.getHeight();
        }
        
        if(chat_lines[i]->time_created + 10500 > gfx::getTicks() || chat_box.active) {
            int alpha = chat_lines[i]->time_created + 10500 - gfx::getTicks();
            if(alpha < 0 || alpha >= 500)
                alpha = 500;
            chat_lines[i]->text_sprite.setColor({255, 255, 255, (unsigned char)((float)alpha / 500.f * 255)});
            chat_lines[i]->text_sprite.render();
        }
    }
}

bool Chat::onKeyDown(gfx::Key key) {
    if(key == gfx::Key::ENTER && chat_box.active) {
        if(!chat_box.getText().empty()) {
            sf::Packet chat_packet;
            chat_packet << ClientPacketType::CHAT << chat_box.getText();
            networking->sendPacket(chat_packet);
            chat_box.setText("");
        }
        chat_box.active = false;
        return true;
    } else if(key == gfx::Key::T && !chat_box.active) {
        chat_box.active = true;
        chat_box.ignore_next_input = true;
        return true;
    } else if(key == gfx::Key::ESCAPE && chat_box.active) {
        chat_box.setText("");
        chat_box.active = false;
        return true;
    }
    return false;
}

void Chat::onEvent(ClientPacketEvent &event) {
    switch(event.packet_type) {
        case ServerPacketType::CHAT: {
            std::string curr_line, whole_message;
            event.packet >> whole_message;
            whole_message.push_back('\n');
            while(!whole_message.empty()) {
                curr_line.push_back(whole_message[0]);
                whole_message.erase(whole_message.begin());
                if(whole_message[0] == '\n') {
                    ChatLine* new_line = new ChatLine;
                    new_line->text = curr_line;
                    new_line->time_created = gfx::getTicks();
                        chat_lines.push_back(new_line);
                    whole_message.erase(whole_message.begin());
                    curr_line = "";
                }
            }
            break;
        }
        default:;
    }
}

void Chat::stop() {
    for(int i = 0; i < chat_lines.size(); i++)
        delete chat_lines[i];
    
    networking->packet_event.removeListener(this);
}
