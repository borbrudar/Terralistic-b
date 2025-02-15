#include "clientPlayers.hpp"
#include "readOpa.hpp"

ClientPlayer::ClientPlayer(const std::string& name, int x, int y, int id) : Player(x, y, name, id) {
    friction = false;
}

void ClientPlayer::loadSkin(const gfx::Surface& skin, ResourcePack* resource_pack){
    player_surface = readOpa(resource_pack->getFile("/misc/skin_template.opa"));
    
    for(int y = 0; y < player_surface.getHeight(); y++)
        for(int x = 0; x < player_surface.getWidth(); x++)
            if(player_surface.getPixel(x, y).a != 0) {
                gfx::Color curr_pixel = player_surface.getPixel(x, y);
                gfx::Color color = skin.getPixel(curr_pixel.b / 8, curr_pixel.g / 8);
                if(y > 17)
                    color.a = 255;
                player_surface.setPixel(x, y, color);
            }
    has_created_surface = true;
}

void ClientPlayers::loadPlayerTexture(ClientPlayer& player) {
    if(&player == main_player) {
        gfx::Surface skin = readOpa(resource_pack->getFile("/misc/skin.opa"));
        player.loadSkin(skin, resource_pack);
        Packet skin_packet;
        skin_packet << ClientPacketType::PLAYER_SKIN << skin.getData() << true;//last arg is whether this is a skin change or not
        networking->sendPacket(skin_packet);
    }
    if(player.has_created_surface) {
        player.player_texture.loadFromSurface(player.player_surface);
        player.has_created_texture = true;
    }
}

void ClientPlayers::render() {
    for(auto i : entities->getEntities())
        if(i->type == EntityType::PLAYER) {
            ClientPlayer* player = (ClientPlayer*)i;
            render(*player);
            if(player->has_moved_x && player->isTouchingGround(blocks) && rand() % 100 < abs(player->getVelocityX()) * 2.5) {
                Particle particle(&walk_particle, player->getX() + int(player->getWidth() / 2), player->getY() + player->getHeight());
                particle.velocity_x = -player->getVelocityX() / 4 + rand() % int(std::abs(player->getVelocityX())) - std::abs(player->getVelocityX()) / 2;
                particle.velocity_y = -(rand() % int(std::abs(player->getVelocityX()) * 2) + std::abs(player->getVelocityX()));
                particles->spawnParticle(particle);
                
            }
        }
}

#define HEADER_MARGIN 4
#define HEADER_PADDING 2

void ClientPlayers::render(ClientPlayer& player_to_draw) {
    if(player_to_draw.isTouchingGround(blocks) && player_to_draw.getVelocityY() == 0)
        player_to_draw.has_jumped = false;
    
    if(player_to_draw.getVelocityX())
        player_to_draw.flipped = player_to_draw.getVelocityX() < 0;
    
    if(player_to_draw.moving_type == MovingType::STANDING)
        player_to_draw.started_moving = 0;
    
    if(player_to_draw.moving_type == MovingType::SNEAKING)
        player_to_draw.texture_frame = 10;
    else if(player_to_draw.has_jumped)
        player_to_draw.texture_frame = 0;
    else if(player_to_draw.moving_type == MovingType::WALKING && player_to_draw.has_moved_x)
        player_to_draw.texture_frame = int(timer.getTimeElapsed() - player_to_draw.started_moving) / 70 % 9 + 1;
    else if(player_to_draw.moving_type == MovingType::SNEAK_WALKING) {
        if(player_to_draw.has_moved_x)
            player_to_draw.texture_frame = int(timer.getTimeElapsed() - player_to_draw.started_moving) / 150 % 6 + 10;
        else
            player_to_draw.texture_frame = 10;
    } else if(player_to_draw.moving_type == MovingType::RUNNING && player_to_draw.has_moved_x)
        player_to_draw.texture_frame = int(timer.getTimeElapsed() - player_to_draw.started_moving) / 80 % 8 + 16;
    else
        player_to_draw.texture_frame = 1;
    
    int player_x = gfx::getWindowWidth() / 2 + player_to_draw.getX() - camera->getX();
    int player_y = gfx::getWindowHeight() / 2 + player_to_draw.getY() - camera->getY();
    if(player_to_draw.has_created_texture)
        player_to_draw.player_texture.render(2, player_x - 4, player_y - 8, {player_to_draw.texture_frame * (PLAYER_WIDTH + 4), 0, (PLAYER_WIDTH + 4), (PLAYER_HEIGHT + 4)}, player_to_draw.flipped);
    else
        player_texture.render(2, player_x - 4, player_y - 8, {player_to_draw.texture_frame * (PLAYER_WIDTH + 4), 0, (PLAYER_WIDTH + 4), (PLAYER_HEIGHT + 4)}, player_to_draw.flipped);

    if(&player_to_draw != main_player) {
        if(!player_to_draw.has_created_text) {
            player_to_draw.name_text.loadFromSurface(gfx::textToSurface(player_to_draw.name));
            player_to_draw.has_created_text = true;
        }
        int header_x = gfx::getWindowWidth() / 2 - player_to_draw.name_text.getTextureWidth() / 2 + player_to_draw.getX() + PLAYER_WIDTH - camera->getX(),
        header_y = gfx::getWindowHeight() / 2 + player_to_draw.getY() - camera->getY() - player_to_draw.name_text.getTextureHeight() - HEADER_MARGIN;
        gfx::RectShape(header_x - HEADER_PADDING, header_y - HEADER_PADDING, player_to_draw.name_text.getTextureWidth() + 2 * HEADER_PADDING, player_to_draw.name_text.getTextureHeight() + 2 * HEADER_PADDING).render(BLACK);
        player_to_draw.name_text.render(1, header_x, header_y);
    }
}

ClientPlayer* ClientPlayers::getPlayerById(int id) {
    for(auto i : entities->getEntities())
        if(i->type == EntityType::PLAYER && i->id == id)
            return (ClientPlayer*)i;
    throw Exception("Player not found by id");
}

void ClientPlayers::onEvent(ClientPacketEvent &event) {
    switch(event.packet_type) {
        case ServerPacketType::PLAYER_JOIN: {
            int x, y;
            int id;
            int moving_type;
            std::string name;
            event.packet >> x >> y >> id >> name >> moving_type;
            ClientPlayer* new_player = new ClientPlayer(name, x, y, id);
            new_player->moving_type = (MovingType)moving_type;
            entities->registerEntity(new_player);
            if(name == username) {
                main_player = new_player;
                main_player->ignore_server_updates = true;
                camera->setX(main_player->getX() + PLAYER_WIDTH);
                camera->setY(main_player->getY() + PLAYER_HEIGHT - 2000);
                camera->jumpToTarget();
            }else if(main_player != nullptr){
                gfx::Surface skin = readOpa(resource_pack->getFile("/misc/skin.opa"));
                Packet skin_packet;
                skin_packet << ClientPacketType::PLAYER_SKIN << skin.getData() << false;
                networking->sendPacket(skin_packet);
            }

            break;
        }
        case ServerPacketType::PLAYER_MOVING_TYPE: {
            int id;
            int moving_type;
            event.packet >> moving_type >> id;
            if(main_player && id != main_player->id) {
                ClientPlayer* player = getPlayerById(id);
                player->moving_type = (MovingType)moving_type;
            }
            
            break;
        }
        case ServerPacketType::PLAYER_JUMPED: {
            int id;
            event.packet >> id;
            if(main_player && id != main_player->id) {
                ClientPlayer* player = getPlayerById(id);
                player->has_jumped = true;
            }
            
            break;
        }
        case ServerPacketType::ENTITY_POSITION: {
            Packet event_packet = event.packet;
            int id;
            int x, y;
            event_packet >> x >> y >> id;
            
            Entity* entity = entities->getEntityById(id);
            if(entity == main_player) {
                Packet packet;
                packet << ClientPacketType::MAIN_PLAYER_POSITION << main_player->getX() << main_player->getY();
                networking->sendPacket(packet);
            }

            break;
        }
        case ServerPacketType::MAIN_PLAYER_POSITION: {
            int x, y;
            event.packet >> x >> y;
            
            entities->setX(main_player, x);
            entities->setY(main_player, y);
            break;
        }
        case ServerPacketType::ENTITY_DELETION: {
            Packet event_packet = event.packet;
            int id;
            event_packet >> id;

            if(main_player && id == main_player->id)
                main_player = nullptr;
            break;
        }
        case ServerPacketType::PLAYER_SKIN:{
            std::vector<unsigned char> buffer;
            buffer.resize(32 * 32 * 4);
            event.packet >> buffer;
            int id;
            event.packet >> id;
            if(id != main_player->id){
                bool new_skin;
                event.packet >> new_skin;
                if(new_skin || !getPlayerById(id)->has_created_surface) {
                    gfx::Surface skin;
                    skin.loadFromBuffer(buffer, 32, 32);
                    getPlayerById(id)->loadSkin(skin, resource_pack);
                    getPlayerById(id)->has_created_texture = false;
                }

            }
        }
        default:;
    }
}
