#include "imageDecoder/png_image_reader.hpp"
#include "imageDecoder/stb_image_decoder.hpp"
#include "core/helper.hpp"

#include <algorithm>
#include <cctype>
#if !defined(FACE_ENGINE_HAS_STB_IMAGE)
#include <cstdio>
#endif
#include <stdexcept>
#include <string>
#if !defined(FACE_ENGINE_HAS_STB_IMAGE)
#include <vector>

#include <png.h>
#endif



bool PngImageReader::can_read(const std::filesystem::path& path) const 
{
    return helper::normalize_extension(path) == ".png";
}

ImageData PngImageReader::read_grayscale(const std::filesystem::path& path) const 
{
    if (!can_read(path)) {
        throw std::runtime_error("png reader does not support: " + path.string());
    }

#if defined(FACE_ENGINE_HAS_STB_IMAGE)
    return detail::StbImageDecoder {}.decode_grayscale(path, "png");
#else
    auto* file = std::fopen(path.string().c_str(), "rb");
    if (file == nullptr) {
        throw std::runtime_error("unable to open png image: " + path.string());
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png == nullptr) {
        std::fclose(file);
        throw std::runtime_error("unable to allocate png read struct");
    }

    png_infop info = png_create_info_struct(png);
    if (info == nullptr) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        std::fclose(file);
        throw std::runtime_error("unable to allocate png info struct");
    }

    if (setjmp(png_jmpbuf(png)) != 0) {
        png_destroy_read_struct(&png, &info, nullptr);
        std::fclose(file);
        throw std::runtime_error("unable to decode png image: " + path.string());
    }

    png_init_io(png, file);
    png_read_info(png, info);

    const auto width = static_cast<std::size_t>(png_get_image_width(png, info));
    const auto height = static_cast<std::size_t>(png_get_image_height(png, info));
    const auto color_type = png_get_color_type(png, info);
    const auto bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) {
        png_set_strip_16(png);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS) != 0) {
        png_set_tRNS_to_alpha(png);
    }
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);

    std::vector<unsigned char> rgba(width * height * 4);
    std::vector<png_bytep> rows(height);
    for (std::size_t row = 0; row < height; ++row) {
        rows[row] = rgba.data() + (row * width * 4);
    }

    png_read_image(png, rows.data());
    png_read_end(png, nullptr);
    png_destroy_read_struct(&png, &info, nullptr);
    std::fclose(file);

    ImageData image {
        .width = width,
        .height = height,
        .pixels = std::vector<unsigned char>(width * height)
    };

    for (std::size_t index = 0; index < image.pixels.size(); ++index) {
        const auto red = static_cast<double>(rgba[index * 4]);
        const auto green = static_cast<double>(rgba[index * 4 + 1]);
        const auto blue = static_cast<double>(rgba[index * 4 + 2]);
        image.pixels[index] = static_cast<unsigned char>(std::clamp(0.299 * red + 0.587 * green + 0.114 * blue, 0.0, 255.0));
    }

    return image;
#endif
}


