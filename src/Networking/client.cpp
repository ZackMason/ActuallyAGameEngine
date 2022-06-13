// Author: Zackery Mason-Blaug
// Date: 2022-06-03
//////////////////////////////////////////////////////////


#include "Networking/client.hpp"
#include "Networking/packet.hpp"

#include <iostream>
#include <exception>
#include <stdexcept>

void client_t::host_connect(const char* addr, uint32_t port){
    set_host(addr, port);
    host_connect();
}
void client_t::host_connect(){
    std::cout << "Client: Connecting to host" << std::endl;
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr)
    {
        throw std::runtime_error("Client: failed to connect to host");
    }
    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Client: connected successfully!" << std::endl;
        on_connect(event);
    }
    else
    {
        std::cout << "Client: Failed to connect!" << std::endl;
        enet_peer_reset(peer);
    }
}
void client_t::disconnect(){
    enet_peer_disconnect(peer, 0);
}

void client_t::on_connect(const ENetEvent& event) {
}

void client_t::on_disconnect(const ENetEvent& event) {
}
void client_t::on_packet(const ENetEvent& event) {

    //std::cout <<"Client: A packet of length " << 
    //    event.packet -> dataLength << " containing \n\n" <<
    //    (const char*)event.packet -> data << " \n\nwas received from " << 
    //    event.peer -> data << " on channel " << event.channelID << std::endl;

    enet_packet_destroy (event.packet);
}

void client_t::on_event(const ENetEvent& event){
    switch (event.type) {
    case ENET_EVENT_TYPE_RECEIVE:
        on_packet(event);
        break;
    case ENET_EVENT_TYPE_CONNECT:
        on_connect(event);
        break;
    case ENET_EVENT_TYPE_DISCONNECT:
        on_disconnect(event);
        break;
    }
}
void client_t::poll_events(int timeout){
    const auto start_timeout = timeout;
    while(timeout --> 0) {
        enet_host_flush(client);
        ENetEvent event;
        while (enet_host_service (client, &event, tick_rate) > 0) {
            on_event(event);
            if (start_timeout != 1)
                timeout = start_timeout;
        }
    }
}

void client_t::pump_event(){
    enet_host_flush(client);
    ENetEvent event;
    if (enet_host_service (client, &event, tick_rate) > 0) {
        on_event(event);
    }
}