// Author: Zackery Mason-Blaug
// Date: 2022-06-19
//////////////////////////////////////////////////////////


#include "Util/string.hpp"

#include <unordered_map>

namespace internal {
    std::unordered_map<string_id_t, std::string> hashed_string_database{};
};

string_id_t save_string(const std::string& str) {
    return save_string(str.c_str());
}

string_id_t save_string(const char* str) {
    const auto id = sid(std::string_view(str));
    internal::hashed_string_database[id] = str;
    return id;
}

const std::string& sid_to_string(string_id_t id) {
    if (internal::hashed_string_database.count(id) == 0) {
        static std::string dummy{"<Unknown String ID>"};
        return dummy;
    }
    return internal::hashed_string_database[id];
}
