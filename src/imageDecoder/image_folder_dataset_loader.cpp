#include "imageDecoder/image_folder_dataset_loader.hpp"

#include <algorithm>
#include <future>
#include <optional>
#include <thread>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
struct PendingSample {
    std::filesystem::path path;
    std::string id;
    std::string subject;
    int label {0};
};

std::size_t resolve_worker_count() 
{
    const auto hardware_threads = std::thread::hardware_concurrency();
    return std::max<std::size_t>(1, hardware_threads == 0 ? 4 : hardware_threads);
}
}  // namespace

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

    std::vector<PendingSample> pending_samples;
    std::unordered_map<std::string, int> labels_by_subject;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file() || !m_image_reader->can_read(entry.path())) {
            continue;
        }

        const auto subject = entry.path().parent_path().filename().string();
        if (subject.empty()) {
            throw std::runtime_error("unable to infer subject label from path: " + entry.path().string());
        }

        const auto [label_it, inserted] = labels_by_subject.try_emplace(subject, static_cast<int>(labels_by_subject.size()));
        (void)inserted; //structured binding

        /*auto label_it = labels_by_subject.find(subject);
         if (label_it == labels_by_subject.end()) {
            const auto label = static_cast<int>(labels_by_subject.size());
            label_it = labels_by_subject.emplace(subject, label).first;
        }*/

        pending_samples.push_back(PendingSample {
            .path = entry.path(),
            .id = std::filesystem::relative(entry.path(), root).string(),
            .subject = subject,
            .label = label_it->second,
        });
    }

    if (pending_samples.empty()) {
        throw std::runtime_error("no supported images found in dataset folder");
    }

    std::sort(
        pending_samples.begin(),
        pending_samples.end(),
        [](const PendingSample& lhs, const PendingSample& rhs) { return lhs.id < rhs.id; }
    );

    std::vector<FaceSample> samples(pending_samples.size());
    std::optional<std::size_t> expected_pixels;
    std::vector<std::pair<std::size_t, std::future<FaceSample>>> in_flight;
    in_flight.reserve(resolve_worker_count());

    for (std::size_t index = 0; index < pending_samples.size(); ++index) {
        const auto& pending = pending_samples[index];
        in_flight.emplace_back(
            index,
            std::async(std::launch::async, [this, pending]() {
                const auto image = m_image_reader->read_grayscale(pending.path);
                return FaceSample {
                    .id = pending.id,
                    .subject = pending.subject,
                    .label = pending.label,
                    .pixels = normalize(image),
                    .embedding = {}
                };
            })
        );

        if (in_flight.size() < resolve_worker_count() && (index + 1) < pending_samples.size()) {
            continue;
        }

        for (auto& [sample_index, task] : in_flight) {
            auto sample = task.get();
            const auto pixel_count = static_cast<std::size_t>(sample.pixels.size());
            if (!expected_pixels.has_value()) {
                expected_pixels = pixel_count;
            }

            if (pixel_count != *expected_pixels) {
                throw std::runtime_error("all images must have the same dimensions");
            }

            samples[sample_index] = std::move(sample);
        }
        in_flight.clear();
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


