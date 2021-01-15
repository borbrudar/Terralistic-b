//
//  packets.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 15/01/2021.
//

#ifndef packets_hpp
#define packets_hpp

#include <vector>

namespace packets {

enum packetType {DISCONNECT, PING, CHUNK, BLOCK_CHANGE};

struct packet {
    packet(packetType type) : type(type) {}
    std::vector<unsigned char> contents;
    packetType type;
    packet& operator<<(char x);
    packet& operator<<(unsigned char x);
    packet& operator<<(short x);
    packet& operator<<(unsigned short x);
    char getChar();
    unsigned char getUChar();
    short getShort();
    unsigned short getUShort();
};

packet getPacket(int socket);
void sendPacket(int socket, packet content);

}

#endif /* packets_hpp */
