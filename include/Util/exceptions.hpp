// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <stdexcept>

class not_implemented_x : public std::logic_error
{
public:
    not_implemented_x() : std::logic_error("Function not yet implemented") { };
};

class runtime_error_x : public std::runtime_error
{
public:
    runtime_error_x(const std::string& t) : std::runtime_error(t) { };
};