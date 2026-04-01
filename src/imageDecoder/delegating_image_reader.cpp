#include "face_engine/infrastructure/io/delegating_image_reader.hpp"

#include <stdexcept>
#include <utility>

namespace face_engine::infrastructure::io {

DelegatingImageReader::DelegatingImageReader(std::vector<std::shared_ptr<application::ports::ImageReader>> readers)
    : readers_(std::move(readers)) {
    if (readers_.empty()) {
        throw std::invalid_argument("delegating image reader requires at least one backend");
    }

    for (const auto& reader : readers_) {
        if (!reader) {
            throw std::invalid_argument("delegating image reader backends must not be null");
        }
    }
}

bool DelegatingImageReader::can_read(const std::filesystem::path& path) const {
    for (const auto& reader : readers_) {
        if (reader->can_read(path)) {
            return true;
        }
    }

    return false;
}

common::ImageData DelegatingImageReader::read_grayscale(const std::filesystem::path& path) const {
    return resolve(path).read_grayscale(path);
}

const application::ports::ImageReader& DelegatingImageReader::resolve(const std::filesystem::path& path) const {
    for (const auto& reader : readers_) {
        if (reader->can_read(path)) {
            return *reader;
        }
    }

    throw std::runtime_error("no image reader registered for: " + path.string());
}

}  // namespace face_engine::infrastructure::io
