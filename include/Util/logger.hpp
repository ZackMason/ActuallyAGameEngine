// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "fmt/core.h"
#include "fmt/color.h"
#include "fmt/os.h"

#include <array>

struct logger_t {
    enum log_level_e {
        INFO,
        ALLOC,
        WARN,
        ERR,
        EXCEPTION,
        CHAI_EXCEPTION,
        GRAPHICS_ERROR,
        SIZE,
    };
    inline static log_level_e level = log_level_e::INFO;

    inline static std::array<fmt::v8::text_style, log_level_e::SIZE> log_color = {
        fg(fmt::color::white) | fmt::emphasis::bold,
        fg(fmt::color::orange) | fmt::emphasis::underline,
        fg(fmt::color::green) | fmt::emphasis::bold,
        fg(fmt::color::crimson) | fmt::emphasis::bold,
        fg(fmt::color::light_golden_rod_yellow) | fmt::emphasis::bold,
        fg(fmt::color::purple) | fmt::emphasis::bold,
        fg(fmt::color::blue_violet) | fmt::emphasis::bold,
    };

    inline static std::array<std::string, log_level_e::SIZE> level_to_string = {
        "Info",
        "Alloc",
        "Warning",
        "Error",
        "Exception",
        "Chai Exception",
        "Graphics Error",
    };

    inline static std::string output{};
    inline static bool console{true};

    static void info(const std::string& message) {
        log(message, log_level_e::INFO);
    }
    static void allocation(const std::string& message) {
        log(message, log_level_e::ALLOC);
    }
    static void warn(const std::string& message) {
        log(message, log_level_e::WARN);
    }
    static void error(const std::string& message) {
        log(message, log_level_e::ERR);
    }
    static void exception(const std::string& message) {
        log(message, log_level_e::EXCEPTION);
    }
    static void chai_exception(const std::string& message) {
        log(message, log_level_e::CHAI_EXCEPTION);
    }
    static void graphics_error(const std::string& message) {
        log(message, log_level_e::GRAPHICS_ERROR);
    }

    static void log(const std::string& message, log_level_e l) {
        if (level <= l) {
            if (console) {
                fmt::print(log_color[l], "{} - {}\n", level_to_string[l], message);
            }
            if (!output.empty()) {
                auto out = fmt::output_file(output, fmt::file::APPEND | fmt::file::WRONLY | fmt::file::CREATE);
                out.print("{} - {}\n", level_to_string[l], message);
            }
        }
    }

    static void open_file(const std::string& path, const std::string& dir) {
        output = fmt::format("{}{}", dir, path);
        auto out = fmt::output_file(output);
        out.print("----- Log -----\n");
    }
    static void set_level(log_level_e l) {
        level = l;
    }

};

