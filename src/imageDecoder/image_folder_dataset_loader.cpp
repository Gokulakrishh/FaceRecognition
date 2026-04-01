#include "imageDecoder/image_folder_dataset_loader.hpp"

#include <filesystem>
#include <stdexcept>
#include <unordered_map>



ImageFolderDatasetLoader::ImageFolderDatasetLoader(std::shared_ptr<ImageReader> image_reader)
    : m_image_reader(std::move(image_reader)) 
{
    if (!m_image_reader) {
        throw std::invalid_argument("image reader must not be null");
    }
}

std::vector<FaceSample> ImageFolderDatasetLoader::load_from_folder(const std::filesystem::path& root) const 
{
    if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
        throw std::invalid_argument("dataset root must be an existing directory");
    }

    std::vector<FaceSample> samples;
    std::unordered_map<std::string, int> labels_by_subject;
    std::size_t expected_pixels = 0;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file() || !m_image_reader->can_read(entry.path())) {
            continue;
        }

        const auto subject = entry.path().parent_path().filename().string();
        if (subject.empty()) {
            throw std::runtime_error("unable to infer subject label from path: " + entry.path().string());
        }

        const auto [label_it, inserted] = labels_by_subject.try_emplace(subject, static_cast<int>(labels_by_subject.size()));


        const auto image = m_image_reader->read_grayscale(entry.path());
        if (expected_pixels == 0) {
            expected_pixels = image.width * image.height;
        }

        if ((image.width * image.height) != expected_pixels) {
            throw std::runtime_error("all images must have the same dimensions");
        }

        samples.push_back(FaceSample {
            .id = std::filesystem::relative(entry.path(), root).string(),
            .subject = subject,
            .label = label_it->second,
            .pixels = normalize(image),
            .embedding = {}
        });
    }

    if (samples.empty()) {
        throw std::runtime_error("no supported images found in dataset folder");
    }

    return samples;
}

Eigen::VectorXd ImageFolderDatasetLoader::normalize(const ImageData& image) const 
{
    Eigen::VectorXd pixels(static_cast<Eigen::Index>(image.pixels.size()));
    for (Eigen::Index index = 0; index < pixels.size(); ++index) {
        pixels(index) = static_cast<double>(image.pixels[static_cast<std::size_t>(index)]) / 255.0;
    }

    return pixels;
}


