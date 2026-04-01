#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace helper {

inline std::string normalize_extension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();

    std::transform(extension.begin(), extension.end(), extension.begin(),
        [](unsigned char value) {
            return static_cast<char>(std::tolower(value));
        }
    );

    return extension;
}
