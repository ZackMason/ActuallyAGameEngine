// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "entt/entt.hpp"

#include "Networking/server.hpp"
#include "Networking/packet.hpp"

#include "Game/components.hpp"

#include "core.hpp"

struct message_t {
    std::string name;
    std::string text;
};

struct game_server_t : public server_t {
    entt::registry players;
    u32 player_count = 0;
    u32 player_connected = 0;
    std::unordered_map<u64, u32> net_ids;

    utl::vector<message_t> message_log;

    void sync_world();
    void create_player();

    void notify(const std::string& message);

    void on_tick() override;
    void on_connect(const ENetEvent& event) override;
    void on_disconnect(const ENetEvent& event) override;
    void on_packet(const ENetEvent& event) override;
};
