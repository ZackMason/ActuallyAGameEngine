// Author: Zackery Mason-Blaug
// Date: 2022-06-03
//////////////////////////////////////////////////////////


#pragma once

#include <string>
#include <cstdint>

#include "enet/enet.h"

struct packet_t {
    std::string data{};
    
    ENetPacket* get() const {
        return packet;
    }

    void send_to_peer(ENetPeer* peer, uint8_t channel = 0) const {
        enet_peer_send(peer, channel, packet);
    }

    void set_data(const std::string& new_data);

    explicit packet_t(const std::string& p_data) : data(p_data) {}

private:
    ENetPacket *packet{
        enet_packet_create(data.c_str(), data.length() + 1, ENET_PACKET_FLAG_RELIABLE)
    };
};

