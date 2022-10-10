// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <stdexcept>
#include <exception>
#include <source_location>

class not_implemented_x : public std::logic_error
{
public:
    std::source_location location;
    not_implemented_x(std::source_location s = std::source_location::current());
};

class runtime_error_x : public std::runtime_error
{
public:
    std::source_location location;
    explicit runtime_error_x(const std::string& t, std::source_location s = std::source_location::current());
};