// Author: Zackery Mason-Blaug
// Date: 2022-06-19
//////////////////////////////////////////////////////////


#pragma once

#include <string>

using string_id_t = size_t;

// https://stackoverflow.com/questions/48896142/is-it-possible-to-get-hash-values-as-compile-time-constants
template <typename Str>
static constexpr string_id_t sid(const Str& toHash) {
    static_assert(sizeof(string_id_t) == 8, "string hash not x64");
	string_id_t result = 0x811c9dc5; // FNV offset basis

	for (char c : toHash) {
		result ^= c;
		result *= 0x01000193; // FNV prime
	}

	return result;
}

template <size_t N>
static constexpr string_id_t sid(char const (&toHash)[N]) {
	return sid(std::string_view(toHash));
}

constexpr string_id_t operator"" _sid(const char* str, std::size_t size) {
    return sid(std::string_view(str));
}

string_id_t save_string(const std::string& str);
string_id_t save_string(const char* str);

const std::string& sid_to_string(string_id_t id);

std::string get_extension(const std::string& s);
bool has_extension(const std::string& s, const std::string& ext);

std::string remove_path(const std::string& str);


