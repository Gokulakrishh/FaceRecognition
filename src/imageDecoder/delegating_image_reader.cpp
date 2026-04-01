#include "imageDecoder/delegating_image_reader.hpp"

#include <stdexcept>
#include <utility>


DelegatingImageReader::DelegatingImageReader(std::vector<std::shared_ptr<ImageReader>> readers)
    : m_readers(std::move(readers)) 
{
    if (m_readers.empty()) {
        throw std::invalid_argument("delegating image reader requires at least one backend");
    }

    for (const auto& reader : m_readers) {
        if (!reader) {
            throw std::invalid_argument("delegating image reader backends must not be null");
        }
    }
}

bool DelegatingImageReader::can_read(const std::filesystem::path& path) const 
{
    for (const auto& reader : m_readers) {
        if (reader->can_read(path)) {
            return true;
        }
    }

    return false;
}

ImageData DelegatingImageReader::read_grayscale(const std::filesystem::path& path) const 
{
    return resolve(path).read_grayscale(path);
}

const ImageReader& DelegatingImageReader::resolve(const std::filesystem::path& path) const 
{
    for (const auto& reader : m_readers) {
        if (reader->can_read(path)) {
            return *reader;
        }
    }

    throw std::runtime_error("no image reader registered for: " + path.string());
}

