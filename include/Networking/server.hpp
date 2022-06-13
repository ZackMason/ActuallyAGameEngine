// Author: Zackery Mason-Blaug
// Date: 2022-06-03
//////////////////////////////////////////////////////////


#pragma once
#include "enet/enet.h"

struct packet_t;
struct server_t {
    enet_uint32 tick_rate{1000};
    ENetAddress address{ENET_HOST_ANY, 1234};
    ENetHost *server{enet_host_create(
        &address, 32, 2, 0, 0
    )};

    void broadcast_packet(const packet_t& packet);

    virtual void poll_events(int timeout = 5);
    virtual void pump_event();

    virtual void on_tick(){};
    virtual void on_event(const ENetEvent& event);
    virtual void on_connect(const ENetEvent& event);
    virtual void on_disconnect(const ENetEvent& event);
    virtual void on_packet(const ENetEvent& event);

    virtual ~server_t(){
        enet_host_destroy(server);
        server = nullptr;
    }
};

