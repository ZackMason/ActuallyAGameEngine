// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Util/exceptions.hpp"
#include <fmt/core.h>

not_implemented_x::not_implemented_x(std::source_location s) 
: std::logic_error("Function not yet implemented"), location(s)
{ 

}

runtime_error_x::runtime_error_x(
    const std::string& t,
    std::source_location s)
: std::runtime_error(t), location(s)
{ 


}
