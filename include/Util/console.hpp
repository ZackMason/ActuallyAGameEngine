// Author: Zackery Mason-Blaug
// Date: 2022-10-13
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/batch2d.hpp"
#include "Graphics/font.hpp"

#include "Util/string.hpp"
#include "Util/allocators.hpp"
#include "Util/color.hpp"

#include "types.hpp"
                    
struct console_t {
    struct command_t {
        std::function<void(void*, std::vector<std::string>&)> fn = [](auto g, auto& args){};
        sid_t name{0};
        u32 args{0};

        command_t(const std::string& n, decltype(fn) _fn, u32 a = 0) :
            fn(_fn), name(save_string(n)), args(a)
        {
        }
        
        command_t(sid_t n, decltype(fn) _fn, u32 a = 0) :
            fn(_fn), name(n), args(a)
        {
        }
    };

    struct message_t {
        std::string text{};
        color32 color;
    };

    v2f window_size() const {
        return ratio * screen_size;
    }

    void clear() {
        message_log.clear();
    }

    bool [[no_discard]] is_open() const {
        return open > 0.5f;
    }

    void toggle_open() {
        target = is_open()?0.0f:1.0f;
    }

    template<typename Batch>
    void draw(Batch& gfx, font_t& font) {

        static f32 a = 0.0f;
        a += 0.1f;

        open = lerp_dt(open, target, 0.5f, 0.1f);

        if (open < 0.05f) {
            return;
        }

        aabb_t<v2f> window_box;
        if (theme.open_style) {
            window_box.expand(v2f(screen_size)*0.5f - v2f{window_size().x*0.5f, open * window_size().y});
            window_box.expand(v2f(screen_size)*0.5f + v2f{window_size().x*0.5f, (open)*window_size().y});
        } else {
            window_box.expand(v2f(screen_size)*v2f{0.5f,1.0f} - v2f{window_size().x*0.5f, open * window_size().y});
            window_box.expand(v2f(screen_size)*v2f{0.5f,1.0f} + v2f{window_size().x*0.5f, -(open*2.0)*window_size().y});
        }

        
        aabb_t<v2f> bg_box = window_box;
        bg_box.min -= v2f{theme.border_size};
        bg_box.max += v2f{theme.border_size};
        
        const v2f text_start = v2f{window_box.min.x, window_box.max.y} + v2f{8.0f, -8.0f};
        
        aabb_t<v2f> input_box = window_box;
        input_box.min += v2f{5.0f};
        input_box.max -= v2f{5.0f};
        input_box.min.y = text_start.y - 3.0f - font.get_glyph(0,0,'L').screen.size().y;

        aabb_t<v2f> messages_box = window_box;
        messages_box.max.y -= input_box.size().y * 2.0f;

        if (open > 0.2f) {
            v2f text_pos = text_start;
            
            if (window_box.contains(text_pos)) {
                gfx.draw_font(input_text, text_pos.x, text_pos.y, font, theme.bg_color);
                if (sin(a) > 0.0f) {
                    gfx.draw_font("_", text_pos.x + font.get_text_size(input_text).x + 2, text_pos.y, font, theme.bg_color);
                }
            }

            text_pos += scroll;
            
            for (auto message = message_log.rbegin(); message != message_log.rend(); ++message) {
                text_pos.y -= font.get_glyph(0,0,'L').screen.size().y + 5.0f;
                if (messages_box.contains(text_pos - v2f{0.0f, font.get_glyph(0,0,'L').screen.size().y})) {
                    gfx.draw_font(message->text, text_pos.x, text_pos.y, font, message->color);
                }
            }
            
        }


        gfx.draw(theme.border_color, input_box);
        gfx.draw(theme.bg_color, window_box);
        gfx.draw(theme.border_color, bg_box);
    }

    void on_enter(void* game) {
        if (!is_open()) return;

        color32 text_color = color::rgba::red;

        const size_t command_index = input_text.find_first_of(' ');
        sid_t command_id = save_string(command_index != std::string::npos ?
            input_text.substr(0, command_index) : 
            input_text
        );

        logger_t::info(sid_to_string(command_id));
            
        for(const auto& command: commands) {
            if (command.name == command_id) {
                std::vector<std::string> args;
                if (command_index != std::string::npos) {
                    auto start = command_index + 1;
                    auto end = input_text.find(' ', start);
                    while(end != std::string::npos) {
                        args.push_back(std::string(input_text.substr(start, end-start)));
                        logger_t::info(args.back());
                        start = end + 1;
                        end = input_text.find(' ', start);
                    }
                    if (start != end) {
                        args.push_back(input_text.substr(start, end));
                    }
                }
                command.fn(game, args);
                text_color = color::rgba::green;
                break;
            }
        } 

        log(input_text, text_color);
        input_text.clear();
    }

    void log(const std::string& text, color32 color = color::rgba::white) {
        message_log.emplace_back(text, color);
    }

    std::vector<message_t> message_log;
    std::vector<command_t> commands;
    std::string input_text;

    struct theme_t {
        color32 bg_color{color::rgba::dark_gray};
        color32 border_color{color::to_color32(color::str_to_rgba("#55fa22dd"))};
        color32 text_color{color::rgba::white};
        int open_style{1};
        f32 border_size{1.0f};
    } theme;

    v2f ratio{0.75f, 0.20f};
    v2f screen_size{};
    v2f scroll{};
    f32 open{0.0f};
    f32 target{0.0f};


    explicit console_t() {
        commands.push_back(console_t::command_t{"help", 
            [&](void* _d, auto& args) {
                for (const auto& cmd: commands) {
                    log(fmt::format("    {}", sid_to_string(cmd.name)), color::rgba::purple);
                }
        }});

        commands.push_back(console_t::command_t{"clear",
            [&](void* _d, auto& args) {
                message_log.clear();
        }});

        commands.push_back(command_t{"set_open_style", [&](void* _d, auto& args) {
            if (args.size() == 1) {
                theme.open_style = std::stoi(args[0]);
            }
        }, 1});

        commands.push_back(command_t{"set_border_size", [&](void* _d, auto& args) {
            if (args.size() == 1) {
                theme.border_size = std::stof(args[0]);
            }
        }, 1});

        commands.push_back(command_t{"set_bg_color", [&](void* _d, auto& args) {
            if (args.size() == 1) {
                theme.bg_color = color::to_color32(color::str_to_rgba(args[0]));
            }
        }, 1});

        commands.push_back(command_t{"set_border_color", [&](void* _d, auto& args) {
            if (args.size() == 1) {
                theme.border_color = color::to_color32(color::str_to_rgba(args[0]));
            }
        }, 1});

        commands.push_back(console_t::command_t{"color", 
            [&](void* _d, auto& args) {
                for (const auto& arg: args) {
                    const color4 cv = color::str_to_rgba(arg);
                    const color32 ci = color::to_color32(cv);
                    log(fmt::format("    {} = {:x} = {}", arg, ci, cv), color::rgba::purple);
                }
        }});

    }


};

