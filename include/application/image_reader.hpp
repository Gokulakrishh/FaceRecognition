#pragma once

#include <filesystem>

#include "core/image_data.hpp"


class ImageReader {
public:
    virtual ~ImageReader() = default;

    [[nodiscard]] virtual bool can_read(const std::filesystem::path& path) const = 0;
    [[nodiscard]] virtual ImageData read_grayscale(const std::filesystem::path& path) const = 0;
};


