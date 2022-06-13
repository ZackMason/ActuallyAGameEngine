// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#include "Game/game_server.hpp"

// Author: Zackery Mason-Blaug
// Date: 2022-06-04
//////////////////////////////////////////////////////////
#define NOMINMAX
#define WIN32_MEAN_AND_LEAN

#include "core.hpp"

#include "Game/game_server.hpp"

#include "Util/entt_json_archive.hpp"

#include "nlohmann/json.hpp"
using nlohmann::json;

#include <glfw/glfw3.h>


void game_server_t::notify(const std::string& message) {
    json j;
    j["type"] = "message";
    j["data"] = message;
    broadcast_packet(packet_t{j.dump()});
}
void game_server_t::sync_world() {
    NJSONOutputArchive json_archive;
    entt::basic_snapshot snapshot(players);
    snapshot.entities(json_archive)
        .component<network_id_c>(json_archive);
    json_archive.Close();

    json j;
    j["type"] = "player_sync";
    j["data"] = json_archive.AsString();

    broadcast_packet(packet_t{j.dump()});
}

void game_server_t::on_disconnect(const ENetEvent& event) {
    auto id = net_ids[(event.peer->address.host << 16) | event.peer->address.port];
    json j;
    j["type"] = "disconnect";
    j["data"] = id;
    player_connected--;

    auto view = players.view<const network_id_c>();
    view.each([&](auto e, const network_id_c& nid) {
        if (id == nid.id) {
            players.destroy(e);
        }
    });

    net_ids.erase((event.peer->address.host << 16) | event.peer->address.port);

    broadcast_packet(packet_t{j.dump()});

    server_t::on_disconnect(event);
}
void game_server_t::on_connect(const ENetEvent& event) {
    std::cout << "Player connected" << std::endl;

    net_ids[(event.peer->address.host << 16) | event.peer->address.port] = player_count;
    json j;
    j["type"] = "connect";
    j["data"] = player_count;

    create_player();
    player_connected++;

    packet_t packet{j.dump()};
    packet.send_to_peer(event.peer);

    server_t::on_connect(event);
}

void game_server_t::create_player(){
    auto p = players.create();
    players.emplace<network_id_c>(p, player_count++);
}

void game_server_t::on_tick()  {
    static f32 last_time = static_cast<f32>(glfwGetTime());
    const f32 time = static_cast<f32>(glfwGetTime());
    const f32 dt = (last_time - time)/1000.0f;
    last_time = time;

    if (player_count)
    {
        auto life_view = players.view<lifetime_c>();
        life_view.each([&](auto e, lifetime_c& l){
            l.time -= dt;
            if (l.time <= 0){
                players.destroy(e);
            }
        });

        sync_world();
    }
}

void game_server_t::on_packet(const ENetEvent& event)  {
    if (event.packet->data){
        json j = json::parse((const char*)event.packet->data);
        auto id = net_ids[(event.peer->address.host << 16) | event.peer->address.port];

        if (j["type"] == "fire") {

        } else if (j["type"] == "move") {

        } else if (j["type"] == "message") {
            auto message = j["data"].get<std::string>();
            fmt::print("Server received: {}\n", message);
            notify(message);
        }
    }
    server_t::on_packet(event);
}