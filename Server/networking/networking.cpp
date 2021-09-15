#include "serverNetworking.hpp"
#include "print.hpp"
#include "compress.hpp"

void Connection::send(sf::Packet& packet) {
    master_packet.append(packet.getData(), packet.getDataSize());
}

void Connection::send(std::vector<char>& data) {
    size_t sent;
    socket->send(&data[0], data.size(), sent);
    if(data.size() != sent)
        print::error("data was not sent properly");
}

sf::Socket::Status Connection::receive(sf::Packet& packet) {
    return socket->receive(packet);
}

std::string Connection::getIpAddress() {
    return socket->getRemoteAddress().toString();
}

void Connection::freeSocket() {
    delete socket;
}

void Connection::flushPacket() {
    if(master_packet.getDataSize()) {
        socket->send(master_packet);
        master_packet.clear();
    }
}

void ServerNetworkingManager::openSocket(unsigned short port) {
    listener.listen(port);
    listener.setBlocking(false);
}

void ServerNetworkingManager::closeSocket() {
    listener.close();
}

void ServerNetworkingManager::sendToEveryone(sf::Packet& packet, Connection* exclusion) {
    for(Connection& connection : connections)
        if(exclusion == nullptr || exclusion->player != connection.player)
            connection.send(packet);
}

void ServerNetworkingManager::checkForNewConnections() {
    static sf::TcpSocket *socket = new sf::TcpSocket;
    while(true) {
        if(listener.accept(*socket) != sf::Socket::NotReady) {
            if(!accept_itself || socket->getRemoteAddress().toString() == "127.0.0.1") {
                socket->setBlocking(false);
                connections.emplace_back(socket);
            } else {
                delete socket;
            }
            socket = new sf::TcpSocket;
        } else
            break;
    }
}

void ServerNetworkingManager::getPacketsFromPlayers() {
    sf::Packet packet;
    for(int i = 0; i < connections.size(); i++) {
        if(connections[i].player) {
            while(true) {
                sf::Socket::Status status = connections[i].receive(packet);
                if(status == sf::Socket::NotReady)
                    break;
                else if(status == sf::Socket::Disconnected) {
                    print::info(connections[i].player->name + " (" + connections[i].getIpAddress() + ") disconnected (" + std::to_string(players->getOnlinePlayers().size() - 1) + " players online)");
                    players->removePlayer(connections[i].player);
                    
                    connections[i].freeSocket();
                    connections.erase(connections.begin() + i);
                    
                    break;
                } else if(status == sf::Socket::Done) {
                    while(!packet.endOfPacket()) {
                        unsigned char packet_type;
                        packet >> packet_type;
                        onPacket(packet, (PacketType)packet_type, connections[i]);
                    }
                }
            }
        } else if(connections[i].receive(packet) != sf::Socket::NotReady) {
            std::string player_name;
            packet >> player_name;
            ServerPlayer* player = players->addPlayer(player_name);
            connections[i].player = player;
            
            sf::Packet welcome_packet;
            welcome_packet << player->getX() << player->getY();
            welcome_packet << blocks->getWidth() << blocks->getHeight();
            
            std::vector<char> map_data = blocks->toData();
            
            map_data = compress(map_data);
            
            welcome_packet << (unsigned int)map_data.size();
            connections[i].send(welcome_packet);
            connections[i].flushPacket();
            connections[i].send(map_data);

            for(ServerPlayer* curr_player : players->getOnlinePlayers()) {
                sf::Packet join_packet;
                join_packet << PacketType::PLAYER_JOIN << curr_player->getX() << curr_player->getY() << curr_player->id << curr_player->name;
                connections[i].send(join_packet);
            }

            for(const ServerItem* item : items->getItems()) {
                sf::Packet item_packet;
                item_packet << PacketType::ITEM_CREATION << item->getX() << item->getY() << item->id << (unsigned char)item->getType();
                connections[i].send(item_packet);
            }

            for(InventoryItem& curr_item : player->inventory.inventory_arr)
                if(curr_item.getType() != ItemType::NOTHING)
                    sendInventoryItemPacket(connections[i], curr_item, curr_item.getType(), curr_item.getStack());
            player->inventory.updateAvailableRecipes();
            
            sf::Packet join_packet;
            join_packet << PacketType::PLAYER_JOIN << player->getX() << player->getY() << player->id << player->name;
            sendToEveryone(join_packet, &connections[i]);

            print::info(player->name + " (" + connections[i].getIpAddress() + ") connected (" + std::to_string(players->getOnlinePlayers().size()) + " players online)");
        }
    }
}

void ServerNetworkingManager::flushPackets() {
    for(Connection& connection : connections)
        connection.flushPacket();
}
