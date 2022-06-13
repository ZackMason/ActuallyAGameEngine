// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <chaiscript/chaiscript.hpp>
#include "Engine/asset_loader.hpp"
#include <entt/entt.hpp>

#include "Engine/window.hpp"

#include <string>
#include <filesystem>

struct script_vm_t {
    static inline bool initialized = false;
    chaiscript::ChaiScript chai;
    
    void init(window_t& window, entt::registry& world, asset_loader_t& loader);
    void eval_file(const std::string& f);
};

struct script_t {
    using chai_ptr_t = std::shared_ptr<chaiscript::dispatch::Dynamic_Object>;
    chai_ptr_t script_object;
    void load_function(chaiscript::ChaiScript& chai);

    std::function<void (chai_ptr_t, entt::entity& e, const f32)> on_update = [](auto state, entt::entity& e, const f32 t){};
    std::string filename = "";
    explicit script_t(const std::string& p_filename) : filename(p_filename) {}
    explicit script_t() = default;
    bool init = false;
};

template<typename Draw>
struct pipeline_script_t {
    using chai_ptr_t = std::shared_ptr<chaiscript::dispatch::Dynamic_Object>;
    chai_ptr_t script_object;

    Draw draw; // draw function

    void execute(f32 dt, chaiscript::ChaiScript& chai) {
        load_function(chai);
        begin_pass(script_object, dt);
        draw();
        end_pass(script_object, dt);
    }

    void reload(chaiscript::ChaiScript& chai){
        init = false;
    }
    
    void load_function(chaiscript::ChaiScript& chai)
    {
        if (init) return;
        init = true;

        //std::filesystem::path p(ASSETS_PATH + filename);
        //std::string class_name = p.stem().string();

        try {
            script_object = chai.eval_file<chai_ptr_t>(ASSETS_PATH + filename);

            begin_pass = chaiscript::boxed_cast<decltype(begin_pass)>(script_object.get()->get_attr("begin_pass"));
            end_pass = chaiscript::boxed_cast<decltype(end_pass)>(script_object.get()->get_attr("end_pass"));
            
        } catch (chaiscript::exception::eval_error &e) {
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
                "Chai Exception caught - {}\n", e.pretty_print());
        } catch (chaiscript::exception::dispatch_error &e) {
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
                "Chai Dispatch Error - {}\n", e.what());
        } catch (std::exception & e) {
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
                "Exception caught - {}\n", e.what());
        }
    }

    std::function<void (chai_ptr_t, const f32)> begin_pass = [](auto state, const f32 t){};
    std::function<void (chai_ptr_t, const f32)> end_pass = [](auto state, const f32 t){};
    std::string filename = "";
    explicit pipeline_script_t(const std::string& p_filename, Draw p_draw) 
        : filename(p_filename), draw(p_draw)
    {}
    explicit pipeline_script_t() = default;
    bool init = false;
};