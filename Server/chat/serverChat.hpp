#pragma once
#include <utility>

#include "serverPlayers.hpp"

class ServerChatEvent {
public:
    ServerChatEvent(ServerPlayer* sender, std::string message) : sender(sender), message(std::move(message)) {}
    ServerPlayer* sender;
    std::string message;
    bool cancelled = false;
};

class ServerChat : public ServerModule, EventListener<ServerPacketEvent> {
    void onEvent(ServerPacketEvent& event) override;
    ServerPlayers* players;
    ServerNetworking* networking;
    
    void init() override;
    void stop() override;
public:
    ServerChat(ServerPlayers* players, ServerNetworking* networking) : players(players), networking(networking) {}
    
    static void sendChat(ServerPlayer* player, const std::string& message);
    
    EventSender<ServerChatEvent> chat_event;
};
