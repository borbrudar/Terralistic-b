#include "clientHealth.hpp"
#include "readOpa.hpp"

void ClientHealth::init() {
    networking->packet_event.addListener(this);
    networking->welcome_packet_event.addListener(this);
}

void ClientHealth::loadTextures() {
    heart_texture.loadFromSurface(readOpa(resource_pack->getFile("/misc/hearts.opa")));
}

void ClientHealth::stop() {
    networking->packet_event.removeListener(this);
    networking->welcome_packet_event.removeListener(this);
}

void ClientHealth::update(float frame_length) {
    enabled = players->getMainPlayer() != nullptr;
}

void ClientHealth::render() {
    int offset = gfx::getWindowWidth() - (std::min(max_health / 4, 10) + 1) * 25 + 15;
    for(int i = max_health / 4 - 1; i >= 0; i--)
        heart_texture.render(2, offset + (i * 25) % 250,  (10 + (i - i % 10)), gfx::RectShape(0, 0 + (std::min(std::max(0, 4 * (i + 1) - health), 4)) * 11, 11, 11));
}

void ClientHealth::onEvent(ClientPacketEvent &event) {
    switch(event.packet_type) {
        case ServerPacketType::HEALTH:
            event.packet >> health;
            break;
        default: break;
    }
}

void ClientHealth::onEvent(WelcomePacketEvent &event) {
    if(event.packet_type == WelcomePacketType::HEALTH)
        event.packet >> health;
}

