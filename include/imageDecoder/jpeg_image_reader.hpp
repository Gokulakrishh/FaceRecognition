// Author: Gokulakrishnan Sivakumar

#pragma once

#include "imageDecoder/image_reader.hpp"



class JpegImageReader final : public ImageReader {
public:
    [[nodiscard]] bool can_read(const std::filesystem::path& path) const override;
    [[nodiscard]] ImageData read_grayscale(const std::filesystem::path& path) const override;
};
