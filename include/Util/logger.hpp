// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "fmt/core.h"
#include "fmt/color.h"
#include "fmt/os.h"

#include <array>
#include <source_location>

struct logger_t {
    enum log_level_e {
        PROFILE,
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
        /* PROFILE */       fg(fmt::color::light_green) | fmt::emphasis::bold,
        /* INFO */          fg(fmt::color::white) | fmt::emphasis::bold,
        /* ALLOC */         fg(fmt::color::orange) | fmt::emphasis::underline,
        /* WARN */          fg(fmt::color::yellow) | fmt::emphasis::bold,
        /* ERR */           fg(fmt::color::crimson) | fmt::emphasis::bold,
        /* EXCEPTION */     fg(fmt::color::light_golden_rod_yellow) | fmt::emphasis::bold,
        /* CHAI_EXCEPTION */fg(fmt::color::purple) | fmt::emphasis::bold,
        /* GRAPHICS_ERROR */fg(fmt::color::blue_violet) | fmt::emphasis::bold,
    };

    inline static std::array<std::string, log_level_e::SIZE> level_to_string = {
        "Profile",
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

    static void profile(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::PROFILE, s);
    }
    static void info(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::INFO, s);
    }
    static void allocation(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::ALLOC, s);
    }
    static void warn(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::WARN, s);
    }
    static void error(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::ERR, s);
    }
    static void exception(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::EXCEPTION, s);
    }
    static void chai_exception(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::CHAI_EXCEPTION, s);
    }
    static void graphics_error(const std::string& message, std::source_location s = std::source_location::current()) {
        log(message, log_level_e::GRAPHICS_ERROR, s);
    }

    static void log(const std::string& message, log_level_e l, std::source_location s = std::source_location::current()) {
        if (level <= l) {
            if (console) {
                fmt::print(log_color[l], "{} - {} - {}:{} {}\n", level_to_string[l], 
                    message, s.file_name(), s.line(), s.function_name());
            }
            if (!output.empty()) {
                auto out = fmt::output_file(output, fmt::file::APPEND | fmt::file::WRONLY | fmt::file::CREATE);
                out.print("{} - {} - {}:{} {}\n", level_to_string[l], 
                    message, s.file_name(), s.line(), s.function_name());
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

