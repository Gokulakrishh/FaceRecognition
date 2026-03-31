#pragma once

#include "application/image_reader.hpp"



class PgmImageReader final : public application::ports::ImageReader {
public:
    [[nodiscard]] bool can_read(const std::filesystem::path& path) const override;
    [[nodiscard]] common::ImageData read_grayscale(const std::filesystem::path& path) const override;
};


