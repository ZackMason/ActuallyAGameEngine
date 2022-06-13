// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include <string>

#include "Util/entt_json_archive.hpp"
#include "Util/vector.hpp"

#include "Math/transform.hpp"

struct network_id_c {
    u32 id{static_cast<u32>(-1)};
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(network_id_c, id);
};

struct lifetime_c {
    f32 time{1.0f};
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(lifetime_c, time);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(glm::vec3, x, y, z);

#if 0
inline void to_json(nlohmann::json& nlohmann_json_j, const m33& nlohmann_json_t) { 
    to_json(nlohmann_json_j["0"], nlohmann_json_t[0]);
    to_json(nlohmann_json_j["1"], nlohmann_json_t[1]);
    to_json(nlohmann_json_j["2"], nlohmann_json_t[2]);
} 
inline void from_json(const nlohmann::json& nlohmann_json_j, m33& nlohmann_json_t) { 
    from_json(nlohmann_json_j.at("0"), nlohmann_json_t[0]);
    from_json(nlohmann_json_j.at("1"), nlohmann_json_t[1]);
    from_json(nlohmann_json_j.at("2"), nlohmann_json_t[2]);
}
#endif


struct transform_c : public transform_t {
    using transform_t::transform_t;
    friend void to_json(nlohmann::json& nlohmann_json_j, const transform_c& nlohmann_json_t) { 
        nlohmann_json_j = {
            {
                "position", {
                    {"x", nlohmann_json_t.origin.x},
                    {"y", nlohmann_json_t.origin.y},
                    {"z", nlohmann_json_t.origin.z}
                }
            },
            {
                "basis", {
                    {
                        "0", {
                            {"x", nlohmann_json_t.basis[0].x},
                            {"z", nlohmann_json_t.basis[0].y},
                            {"y", nlohmann_json_t.basis[0].z},
                        }
                    },
                    {
                        "1", {
                            {"x", nlohmann_json_t.basis[1].x},
                            {"y", nlohmann_json_t.basis[1].y},
                            {"z", nlohmann_json_t.basis[1].z}
                        }
                    },
                    {
                        "2", {
                            {"x", nlohmann_json_t.basis[2].x},
                            {"y", nlohmann_json_t.basis[2].y},
                            {"z", nlohmann_json_t.basis[2].z}
                        }
                    },
                }
            }
        };
    } 
    friend void from_json(const nlohmann::json& nlohmann_json_j, transform_c& nlohmann_json_t) { 
        nlohmann_json_t.origin.x = nlohmann_json_j.at("origin").at("x");
        nlohmann_json_t.origin.y = nlohmann_json_j.at("origin").at("y");
        nlohmann_json_t.origin.z = nlohmann_json_j.at("origin").at("z");

        nlohmann_json_t.basis[0].x = nlohmann_json_j.at("basis").at("0").at("x");
        nlohmann_json_t.basis[0].y = nlohmann_json_j.at("basis").at("0").at("y");
        nlohmann_json_t.basis[0].z = nlohmann_json_j.at("basis").at("0").at("z");

        nlohmann_json_t.basis[1].x = nlohmann_json_j.at("basis").at("1").at("x");
        nlohmann_json_t.basis[1].y = nlohmann_json_j.at("basis").at("1").at("y");
        nlohmann_json_t.basis[1].z = nlohmann_json_j.at("basis").at("1").at("z");

        nlohmann_json_t.basis[2].x = nlohmann_json_j.at("basis").at("2").at("x");
        nlohmann_json_t.basis[2].y = nlohmann_json_j.at("basis").at("2").at("y");
        nlohmann_json_t.basis[2].z = nlohmann_json_j.at("basis").at("2").at("z");
    }
};

struct mesh_c {
    std::string mesh;
    utl::vector<std::string> shaders;
    utl::vector<std::string> textures;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(mesh_c, mesh, shaders, textures);
};