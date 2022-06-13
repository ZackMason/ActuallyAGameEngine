// Author: Zackery Mason-Blaug
// Date: 2022-06-03
//////////////////////////////////////////////////////////


#include "Networking/server.hpp"
#include "Networking/packet.hpp"

#include <iostream>

void server_t::broadcast_packet(const packet_t& packet) {
    enet_host_broadcast(server, 0, packet.get());
}

void server_t::on_connect(const ENetEvent& event) {
    std::cout << "Server: A new client connected from " <<
        event.peer->address.host << ":" << event.peer->address.port <<  std::endl;
    //broadcast_packet(packet_t{"Welcome to the server: " + std::to_string(event.peer->address.host)});
}

void server_t::on_disconnect(const ENetEvent& event) {
    std::cout << "Server: A client disconnected from " <<
        event.peer->address.host << ":" << event.peer->address.port << std::endl;
        event.peer->data = NULL;
}
void server_t::on_packet(const ENetEvent& event) {
    //char buffer[64];
    //int r = enet_address_get_host_ip(&event.peer->address, buffer, 64);

    //std::cout << "Server: Received a packet of length " << 
    //    event.packet->dataLength << " containing \n\n" << (const char*)event.packet->data <<
    //    "\n\nwas received from " << (r == 0 ? (const char*)buffer : (void*)event.peer->address.host) << ":" <<
    //    event.peer->address.port << std::endl;
    
    enet_packet_destroy(event.packet);
}

void server_t::on_event(const ENetEvent& event) {
    switch (event.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
        on_connect(event);
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        on_packet(event);
        break;
    case ENET_EVENT_TYPE_DISCONNECT:
        on_disconnect(event);
        break;
    default:
        std::cout << "Server: Unknown packet type " << event.type << std::endl;
        break;
    }
}

void server_t::poll_events(int timeout) {
    const auto start_timeout = timeout;
    while (timeout --> 0) {
        enet_host_flush(server);
        on_tick();
        ENetEvent event;
        while (enet_host_service(server, &event, tick_rate) > 0) {
            on_event(event);
            if (start_timeout != 1)
                timeout = start_timeout;
        }    
        enet_host_flush(server);
    }
}
void server_t::pump_event() {
    enet_host_flush(server);
    on_tick();
    ENetEvent event;
    while(enet_host_service(server, &event, tick_rate) > 0) {
        on_event(event);
    }    
    enet_host_flush(server);
}
