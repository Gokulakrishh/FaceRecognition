#pragma once

#include <filesystem>
#include <string_view>

#include "core/image_data.hpp"



[[nodiscard]] ImageData read_grayscale_with_stb(
    const std::filesystem::path& path,
    std::string_view format_name
);

