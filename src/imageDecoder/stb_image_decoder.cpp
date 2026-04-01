#include "imageDecoder/stb_image_decoder.hpp"

#include <stdexcept>
#include <string>
#include <vector>

#if defined(FACE_ENGINE_HAS_STB_IMAGE)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



ImageData StbImageDecoder::decode_grayscale(const std::filesystem::path& path, const std::string_view format_name) const 
{
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 1);
    if (pixels == nullptr) {
        throw std::runtime_error(
            "unable to decode " + std::string(format_name) + " image: " + path.string() + " (" + stbi_failure_reason() +
            ")"
        );
    }

    ImageData image {
        .width = static_cast<std::size_t>(width),
        .height = static_cast<std::size_t>(height),
        .pixels = std::vector<unsigned char>(pixels, pixels + (static_cast<std::size_t>(width) * static_cast<std::size_t>(height)))
    };

    stbi_image_free(pixels);
    return image;
}

#else

ImageData StbImageDecoder::decode_grayscale(const std::filesystem::path&, const std::string_view) const 
{
    throw std::runtime_error("stb_image backend is not enabled");
}


#endif
