#pragma once

#include <memory>
#include <vector>

#include "imageDecoder/image_reader.hpp"


class DelegatingImageReader final : public ImageReader {
public:
    explicit DelegatingImageReader(std::vector<std::shared_ptr<ImageReader>> readers);

    [[nodiscard]] bool can_read(const std::filesystem::path& path) const override;
    [[nodiscard]] ImageData read_grayscale(const std::filesystem::path& path) const override;

private:
    [[nodiscard]] const ImageReader& resolve(const std::filesystem::path& path) const;

    std::vector<std::shared_ptr<ImageReader>> m_readers;
};


