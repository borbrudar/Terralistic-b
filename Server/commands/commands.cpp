#include "commands.hpp"
#include "print.hpp"
#include "vector"

void Commands::onEvent(ServerChatEvent& event) {
    if(event.message[0] == '/') {
        startCommand(event.message, event.sender);
        event.cancelled = true;
    }
}

void Commands::init() {
    chat->chat_event.addListener(this);
}

void Commands::stop() {
    chat->chat_event.removeListener(this);
}

void Commands::startCommand(std::string message, ServerPlayer* player) {
    std::vector<std::string> seperated;
    size_t pos = message.find(' ');
    while (pos != std::string::npos) {
        seperated.push_back(message.substr(0, pos));
        message.erase(0, pos + 1);
        pos = message.find(' ');
    }
    seperated.push_back(message.substr(0, pos));

    if(seperated[0] == "/setBlock") {
        formatLocation(seperated, player, 1);
        formatBlockType(seperated[3]);
        changeBlock(seperated);
    }else if(seperated[0] == "/tp"){
        if(seperated.size() == 2 || players->getPlayerByName(seperated[1]) == nullptr){
            formatLocation(seperated, player, 1);
            teleport(player, std::stoi(seperated[1]), std::stoi(seperated[2]));
        }else {
            formatLocation(seperated, player, 2);
            teleport(players->getPlayerByName(seperated[1]), std::stoi(seperated[2]), std::stoi(seperated[3]));
        }
    }else if(seperated[0] == "/give"){
        formatItemType(seperated[1]);
        giveItem(seperated, player);
    }

}

void Commands::changeBlock(std::vector<std::string>& message) {
    BlockType block = (BlockType)std::stoi(message[3]);
    
    blocks->setBlockType(std::stoi(message[1]), blocks->getHeight() - std::stoi(message[2]), block);
}

void Commands::formatLocation(std::vector<std::string>& message, ServerPlayer* player, unsigned char start_format) {
    int block_x;

    if(players->getPlayerByName(message[start_format]) != nullptr){
        message.insert(message.begin() + start_format + 1, std::to_string(blocks->getHeight() - players->getPlayerByName(message[start_format])->getY() / 16));
        message[start_format] = std::to_string(players->getPlayerByName(message[start_format])->getX() / 16);
        return;
    }
    if(message[start_format].at(0) == '~') {
        block_x = player->getX() / 16;
        if(message[start_format].size() > 1) {
            message[start_format].erase(0, 1);
            block_x += std::stoi(message[start_format]);
        }
        message[start_format] = std::to_string(block_x);
    }
    start_format++;
    if(message[start_format].at(0) == '~') {
        block_x = blocks->getHeight() - player->getY() / 16;
        if(message[start_format].size() > 1) {
            message[start_format].erase(0, 1);
            block_x += std::stoi(message[start_format]);
        }
        message[start_format] = std::to_string(block_x);
    }
}

void Commands::formatBlockType(std::string &type) {
    if(!(type.at(0) >= '0' && type.at(0) <= '9'))
        type = std::to_string((int)getBlockTypeByName(type));
}

void Commands::formatItemType(std::string &type) {
    if(!(type.at(0) >= '0' && type.at(0) <= '9'))
        type = std::to_string((int)getItemTypeByName(type));
}

void Commands::teleport(ServerPlayer* player, int x, int y) {
    y *= -1;
    y += blocks->getHeight();
    entities->setX(player, x * 16);
    entities->setY(player, y * 16);
}

void Commands::giveItem(std::vector<std::string> &message, ServerPlayer* player) {
    if(message.size() == 2)
        message.emplace_back("1");
    player->inventory.addItem((ItemType) std::stoi(message[1]), std::stoi(message[2]));
}
