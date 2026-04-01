// Author: Gokulakrishnan Sivakumar

#pragma once

#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <istream> 
#include <limits>     
#include <stdexcept>

namespace helper {

inline std::string normalize_extension(const std::filesystem::path& path) 
{
    std::string extension = path.extension().string();

    std::transform(extension.begin(), extension.end(), extension.begin(),
        [](unsigned char value) {
            return static_cast<char>(std::tolower(value));
        }
    );

    return extension;
}

inline std::string read_token(std::istream& stream) 
{
    std::string token;

    while (stream >> token) {
        if (!token.empty() && token[0] == '#') {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        return token;
    }

    throw std::runtime_error("unexpected end of pgm file");
}

inline std::uint16_t resolve_port(const int argc, char** argv)
{
    if (argc >= 2) {
        return static_cast<std::uint16_t>(std::stoul(argv[1]));
    }

    if (const char* port_value = std::getenv("PORT"); port_value != nullptr) {
        return static_cast<std::uint16_t>(std::stoul(port_value));
    }

    return 8080;
}

inline std::string trim(const std::string& value)
{
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }

    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

}
