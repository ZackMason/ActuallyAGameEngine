#include <fmt/core.h>
#include <fmt/color.h>

#include <entt/entt.hpp>

#include <cassert>
#include <exception>

#include "core.hpp"

#include "Engine/window.hpp"
#include "Engine/asset_loader.hpp"
#include "Engine/camera.hpp"

#include "Graphics/texture2d.hpp"
#include "Graphics/shader.hpp"
#include "Graphics/static_mesh.hpp"

static size_t tests_run = 0;
static size_t tests_passed = 0;

struct test_failed : std::exception {
    std::string message;
    test_failed(std::string&& text){ 
        message = std::move(text);
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

constexpr auto throw_assert(const bool b, std::string&& text) -> auto {
    if (!b) throw test_failed(std::move(text));
}

constexpr auto throw_assert(const bool b) -> auto {
    throw_assert(b, "Assert Failed");
}


#define TEST_ASSERT( x ) throw_assert(x, fmt::format("TEST FAILED: " #x "\n{}:{}", __FILE__, __LINE__));

constexpr auto foo(const auto& list) -> auto {
    for (const auto& i: list) {
        fmt::print("{}\n", i);
        foo(i);
    }
}

template <typename Fn>
auto run_test(const char* name, const Fn& test) -> auto {
    ++tests_run;
    try {
        test();
        ++tests_passed;
        fmt::print(fg(fmt::color::cyan) | fmt::emphasis::bold,
            "{} - Passed\n", name);
    } catch (std::exception & e) {
        fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold,
            "{} - Failed\n\n", name);
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
            "{}\n\n", e.what());
    }
}

int main(int argc, char** argv) {
    using namespace std::string_literals;

    run_test("equal", [](){
        TEST_ASSERT(1 == 1);
    });

    run_test("window_create", [](){
        window_t window;
        TEST_ASSERT(!window.is_open());
        window.open_window();
        TEST_ASSERT(window.is_open());
        window.close_window();
        TEST_ASSERT(!window.is_open());
        window.open_window();
        TEST_ASSERT(window.is_open());
        window.close_window();
        TEST_ASSERT(!window.is_open());
    });

    run_test("texture", [](){
        texture2d_t texture{"textures/UVgrid.png", ASSETS_PATH};
//        TEST_ASSERT(texture.id != 0);
        TEST_ASSERT(texture.width != 0);
        TEST_ASSERT(texture.height != 0);
        texture.destroy();
        TEST_ASSERT(texture.id == 0);
    });

    run_test("shader_bad_load", [](){
        bool passed = true;

        try {
            shader_t shader("fail shader", {"not a real path"}, ASSETS_PATH);
            passed = false;
        } catch (...) {
            passed = true;
        }

        TEST_ASSERT(passed == true);
    });

    run_test("mesh_load", [](){
        static_mesh_t mesh = static_mesh_t::from_obj("models/sphere.obj", ASSETS_PATH);
    });


    run_test("asset_loader", [](){
        asset_loader_t loader;
        
        static const std::string sphere_path{"models/sphere.obj"s};
        TEST_ASSERT(loader.get_count<static_mesh_t>(sphere_path) == 0);
        {
            auto m1 = loader.get_static_mesh(sphere_path);

            TEST_ASSERT(loader.get_count<static_mesh_t>(sphere_path) == 1);
            {
                auto m2 = loader.get_static_mesh(sphere_path);
                TEST_ASSERT(loader.get_count<static_mesh_t>(sphere_path) == 2);
                auto m3 = m2;
                TEST_ASSERT(loader.get_count<static_mesh_t>(sphere_path) == 3);
                auto m4(std::move(m3));
                TEST_ASSERT(loader.get_count<static_mesh_t>(sphere_path) == 4);
            }
        }
        //fmt::print("count: {}\n",loader.get_count<static_mesh_t>(sphere_path));
        TEST_ASSERT(loader.get_count<static_mesh_t>(sphere_path) == 0);
        auto m1 = loader.get_static_mesh(sphere_path);
        TEST_ASSERT(m1.get_count() == 1);
    });
    run_test("entt_asset_loader", [](){
        window_t window;
        window.open_window();
        entt::registry world;
        asset_loader_t loader;

        auto mesh_handle = loader.get_static_mesh("models/sphere.obj");
        auto base_shader_handle = loader.get_shader("base", {"shaders/base.vs", "shaders/base.fs"});

        for (int i = 0; i < 10; i++){
            auto e = world.create();
            world.emplace<v3f>(e,  10.0f, 10.0f, 10.0f);
            world.emplace<resource_handle_t<static_mesh_t>>(e, mesh_handle);
            world.emplace<resource_handle_t<shader_t>>(e, base_shader_handle);
        }
        TEST_ASSERT(mesh_handle.get_count() == 11);
        TEST_ASSERT(base_shader_handle.get_count() == 11);

        world.clear();

        TEST_ASSERT(mesh_handle.get_count() == 1);
        TEST_ASSERT(base_shader_handle.get_count() == 1);
        
        window.close_window();
    });

    const auto fmt_color = tests_passed == 0 ? fg(fmt::color::crimson) : 
                            tests_passed == tests_run ? fg(fmt::color::green) : 
                                                        fg(fmt::color::yellow);
    fmt::print(fmt_color | fmt::emphasis::bold,
        "===================================================\n");
    fmt::print(fmt_color | fmt::emphasis::bold,
        "{} / {} Tests Passed\n", tests_passed, tests_run);
    return 0;
}
