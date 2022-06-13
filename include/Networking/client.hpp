// Author: Zackery Mason-Blaug
// Date: 2022-06-03
//////////////////////////////////////////////////////////


#pragma once


#include <cstdint>
#include <cstring>

#include "enet/enet.h"

struct client_t {
    enet_uint32 tick_rate{1000};
    ENetAddress address;
    ENetHost *client{enet_host_create(
        nullptr, 2, 1, 0, 0
    )};
    ENetPeer *peer{nullptr};

    void set_host(const char* addr, uint32_t port){
        enet_address_set_host(&address, addr);
        address.port = port;
    }

    // must set_host first
    void host_connect();
    void host_connect(const char* addr, uint32_t port);


    virtual void on_event(const ENetEvent& event);
    virtual void on_connect(const ENetEvent& event);
    virtual void on_disconnect(const ENetEvent& event);
    virtual void on_packet(const ENetEvent& event);

    void poll_events(int timeout = 5);
    void pump_event();

    void disconnect();

    virtual ~client_t(){
        enet_host_destroy(client);
    }
};

