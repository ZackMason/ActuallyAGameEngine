#include "core.hpp"

#include <execution>

#define WIN32_MEAN_AND_LEAN
#include <enet/enet.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <fmt/core.h>
#include <fmt/color.h>

#include <entt/entt.hpp>

#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Engine/window.hpp"
#include "Engine/asset_loader.hpp"
#include "Engine/orbit_fly_camera.hpp"
#include "Engine/ui_layer.hpp"

#include "Scripting/script.hpp"

#include "Graphics/buffer.hpp"
#include "Graphics/vertex_array.hpp"
#include "Graphics/shader.hpp"
#include "Graphics/static_mesh.hpp"
#include "Graphics/screen.hpp"
#include "Graphics/framebuffer.hpp"

#include "Math/transform.hpp"

#include "Game/game.hpp"
#include "Game/main_menu.hpp"
#include "Game/server_app.hpp"

int main(int argc, char** argv)
{
    enet_initialize();
    atexit(enet_deinitialize);

    srand((u32)time(nullptr));
    auto randf = []() {
        return (f32(rand()) / f32(RAND_MAX)) * 2.0f - 1.0f;
    };

    try {
        auto is_server = false;
        
        {
            main_menu_t main_menu;
            is_server = main_menu.run_menu();
        }

        if (is_server) {
            server_app_t game;
            game.run();
        } else {
            game_t game;
            game.run();
        }


    } catch (chaiscript::exception::eval_error &e) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
            "Chai Exception caught - {}\n", e.pretty_print());
    }
    catch (chaiscript::exception::dispatch_error &e) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
            "Chai Dispatch Error - {}\n", e.what());
    } catch (std::exception & e) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
            "Exception caught - {}\n", e.what());
    }

    fmt::print(fg(fmt::color::white_smoke) | fmt::emphasis::bold,
            "Closing down\n");

    return 0;
}