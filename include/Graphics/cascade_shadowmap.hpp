// Author: Zackery Mason-Blaug
// Date: 2022-06-29
//////////////////////////////////////////////////////////


#pragma once

#include "Engine/camera.hpp"

#include "Util/vector.hpp"

#include "types.hpp"

struct cascade_shadowmap_t {
    unsigned int fbo_id{};
    unsigned int depth_maps_id{};
    std::array<f32, 8> levels;

    void bind();
    void unbind();
    void bind_depth_map(unsigned int slot = 2);


    explicit cascade_shadowmap_t();

    utl::vector<v4f> get_corners(const camera_t& camera) {
        return camera.get_frustum_corners();
    }

    utl::vector<glm::mat4> get_light_matrices(const v3f& light_dir, 
        const camera_t& camera)
    {
        levels = {
            camera.f / 800.0f, 
            camera.f / 400.0f, 
            camera.f / 200.0f, 
            camera.f / 100.0f, 
            camera.f / 50.0f, 
            camera.f / 25.0f, 
            camera.f / 10.0f, 
            camera.f / 2.0f
        };
        std::vector<glm::mat4> ret;
        for (size_t i = 0; i < levels.size() + 1; ++i)
        {
            if (i == 0)
            {
                ret.push_back(get_light_space_matrix(light_dir, camera, camera.n, levels[i]));
            }
            else if (i < levels.size())
            {
                ret.push_back(get_light_space_matrix(light_dir, camera, levels[i - 1], levels[i]));
            }
            else
            {
                ret.push_back(get_light_space_matrix(light_dir, camera, levels[i - 1], camera.f));
            }
        }
        return ret;
    }

    glm::mat4 get_light_space_matrix(const v3f& light_dir, const camera_t& camera, const float near_plane, const float far_plane) {
        const auto proj = glm::perspective(
            camera.fov, camera.width/camera.height, near_plane, far_plane
        );
        camera_t light_camera = camera;
        light_camera.projection = proj;
        const auto corners = get_corners(light_camera);
        v3f center{0};
        for (const auto& v: corners) {
            center += glm::vec3(v);
        }
        center /= static_cast<f32>(corners.size());
        light_camera.position = center + light_dir;
        light_camera.look_at(center);

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();
        for (const auto& v : corners)
        {
            const auto trf = light_camera.view * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Tune this parameter according to the scene
        constexpr float zMult = 10.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        light_camera.projection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

        return light_camera.view_projection();
    }

};

