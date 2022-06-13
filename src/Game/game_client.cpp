// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////

#define NOMINMAX
#define WIN32_MEAN_AND_LEAN
#include "Game/game_client.hpp"

#include "nlohmann/json.hpp"

using nlohmann::json;

#include "Util/entt_json_archive.hpp"

#include "core.hpp"

void game_client_t::on_packet(const ENetEvent& event) {
    if (event.packet -> data != nullptr)
    {
        std::string data = (const char*)event.packet -> data;
        json j = json::parse(data);

        if (j["type"] == "player_sync")
        {
            NJSONInputArchive json_in((j["data"]));
            players.clear();
            entt::basic_snapshot_loader loader(players);
            loader.entities(json_in)
                .component<network_id_c>(json_in);
            
        }
        else if (j["type"] == "disconnect")
        {
            auto view = players.view<const network_id_c>();
            view.each([&](auto e, const network_id_c& nid) {
                if (j["data"] == nid.id) {
                    players.destroy(e);
                }
            });
        }
        else if (j["type"] == "connect")
        {
            fmt::print("Connection received\n");
            net_id = j["data"];
        }
        else if (j["type"] == "message") {
            auto message = j["data"];
            message_queue.push_back(message);
        }
    }
    client_t::on_packet(event);
}