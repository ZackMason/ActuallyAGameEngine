// Author: Zackery Mason-Blaug
// Date: 2022-06-19
//////////////////////////////////////////////////////////


#pragma once

#include <string>
#include <iostream>
#include <chrono>

#include "fmt/color.h"
#include "fmt/core.h"

#include "Util/logger.hpp"

struct profile_t {
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    bool stopped = false;

    profile_t(const std::string& p_name) :
    name(p_name)
    {
        start = std::chrono::high_resolution_clock::now();
    }
    
    ~profile_t()
    {
        if (stopped) return;
        auto stop = std::chrono::high_resolution_clock::now();
        logger_t::profile(fmt::format("{}: {}ms", name, std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()));
    }
    
    auto end()
    {
        stopped = true;
        auto stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    }
};

