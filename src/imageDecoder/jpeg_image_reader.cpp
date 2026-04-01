// Author: Gokulakrishnan Sivakumar

#include "imageDecoder/jpeg_image_reader.hpp"
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
#endif

#if !defined(FACE_ENGINE_HAS_STB_IMAGE)
extern "C" {
#include <jpeglib.h>
}
#endif



bool JpegImageReader::can_read(const std::filesystem::path& path) const 
{
    const auto extension = helper::normalize_extension(path);
    return extension == ".jpg" || extension == ".jpeg";
}

ImageData JpegImageReader::read_grayscale(const std::filesystem::path& path) const 
{
    if (!can_read(path)) {
        throw std::runtime_error("jpeg reader does not support: " + path.string());
    }

#if defined(FACE_ENGINE_HAS_STB_IMAGE)
    return detail::StbImageDecoder {}.decode_grayscale(path, "jpeg");
#else
    auto* file = std::fopen(path.string().c_str(), "rb");
    if (file == nullptr) {
        throw std::runtime_error("unable to open jpeg image: " + path.string());
    }

    jpeg_decompress_struct info {};
    jpeg_error_mgr error_manager {};
    info.err = jpeg_std_error(&error_manager);
    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, TRUE);
    jpeg_start_decompress(&info);

    ImageData image {
        .width = static_cast<std::size_t>(info.output_width),
        .height = static_cast<std::size_t>(info.output_height),
        .pixels = std::vector<unsigned char>(
            static_cast<std::size_t>(info.output_width) * static_cast<std::size_t>(info.output_height)
        )
    };

    const auto row_stride = static_cast<std::size_t>(info.output_width) * static_cast<std::size_t>(info.output_components);
    std::vector<unsigned char> row_buffer(row_stride);

    while (info.output_scanline < info.output_height) {
        auto* row_pointer = row_buffer.data();
        jpeg_read_scanlines(&info, &row_pointer, 1);

        const auto row_index = static_cast<std::size_t>(info.output_scanline - 1);
        for (std::size_t column = 0; column < image.width; ++column) {
            unsigned char gray = 0;

            if (info.output_components == 1) {
                gray = row_buffer[column];
            } else {
                const auto red = static_cast<double>(row_buffer[column * info.output_components]);
                const auto green = static_cast<double>(row_buffer[column * info.output_components + 1]);
                const auto blue = static_cast<double>(row_buffer[column * info.output_components + 2]);
                gray = static_cast<unsigned char>(std::clamp(0.299 * red + 0.587 * green + 0.114 * blue, 0.0, 255.0));
            }

            image.pixels[row_index * image.width + column] = gray;
        }
    }

    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);
    std::fclose(file);

    return image;
#endif
}

