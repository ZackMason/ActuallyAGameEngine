// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "entt/entt.hpp"

#include "Networking/client.hpp"
#include "Networking/packet.hpp"

#include "Game/components.hpp"

struct game_client_t : public client_t {
    entt::registry players;
    u32 net_id{static_cast<u32>(-1)};
    u32 player_count{0};

    std::vector<std::string> message_queue;

    void on_packet(const ENetEvent& event) override;
};

