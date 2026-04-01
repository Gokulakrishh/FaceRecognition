#pragma once

#include <filesystem>
#include <string_view>

#include "core/image_data.hpp"


class StbImageDecoder {
public:

[[nodiscard]] ImageData decode_grayscale(const std::filesystem::path& path, std::string_view format_name) const;

};
