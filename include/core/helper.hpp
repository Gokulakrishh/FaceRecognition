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

}
