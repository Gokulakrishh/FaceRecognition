#include "imageDecoder/pgm_image_reader.hpp"
#include "core/helper.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>



bool PgmImageReader::can_read(const std::filesystem::path& path) const 
{
    return helper::normalize_extension(path) == ".pgm";
}

ImageData PgmImageReader::read_grayscale(const std::filesystem::path& path) const 
{
    if (!can_read(path)) {
        throw std::runtime_error("pgm reader does not support: " + path.string());
    }

    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("unable to open image: " + path.string());
    }

    const auto pgmFormat = helper::read_token(input);
    const auto width = static_cast<std::size_t>(std::stoul(helper::read_token(input)));
    const auto height = static_cast<std::size_t>(std::stoul(helper::read_token(input)));
    const auto max_value = std::stoul(helper::read_token(input));

    if (max_value == 0 || max_value > 255) {
        throw std::runtime_error("unsupported pgm max value in: " + path.string());
    }

    ImageData image {
        .width = width,
        .height = height,
        .pixels = std::vector<unsigned char>(width * height)
    };

    if (pgmFormat == "P2") {
        for (std::size_t index = 0; index < image.pixels.size(); ++index) {
            image.pixels[index] = static_cast<unsigned char>(std::stoul(helper::read_token(input)));
        }
        return image;
    }

    if (pgmFormat != "P5") {
        throw std::runtime_error("unsupported pgm format in: " + path.string());
    }

    input.get();
    input.read(reinterpret_cast<char*>(image.pixels.data()), static_cast<std::streamsize>(image.pixels.size()));
    if (input.gcount() != static_cast<std::streamsize>(image.pixels.size())) {
        throw std::runtime_error("unable to read pixel data from: " + path.string());
    }

    return image;
}


