#include "application/recognition_service.hpp"

#include <stdexcept>
#include <utility>


RecognitionService::RecognitionService(std::shared_ptr<FaceRepository> repository, std::shared_ptr<FaceDatasetLoader> dataset_loader, std::shared_ptr<ImageReader> image_reader)
    : m_repository(std::move(repository)),
      m_dataset_loader(std::move(dataset_loader)),
      m_image_reader(std::move(image_reader)) 
{
    if (!m_repository || !m_dataset_loader || !m_image_reader) {
        throw std::invalid_argument("recognition service dependencies must not be null");
    }
}

void RecognitionService::train(std::vector<FaceSample> samples, const std::size_t components) 
{
    if (samples.empty()) {
        throw std::invalid_argument("training samples must not be empty");
    }

    const auto training_matrix = build_training_matrix(samples);
    m_pca_model.fit(training_matrix, components);

    m_repository->clear();
    for (auto& sample : samples) {
        sample.embedding = m_pca_model.project(sample.pixels);
        m_repository->save(std::move(sample));
    }
}

void RecognitionService::train_from_folder(const std::filesystem::path& dataset_root, const std::size_t components) 
{
    train(m_dataset_loader->load_from_folder(dataset_root), components);
}

void RecognitionService::register_sample(FaceSample sample) 
{
    sample.embedding = project(sample.pixels);
    m_repository->save(std::move(sample));
}

Eigen::VectorXd RecognitionService::project(const Eigen::VectorXd& pixels) const 
{
    return m_pca_model.project(pixels);
}

Eigen::VectorXd RecognitionService::project_image(const std::filesystem::path& image_path) const 
{
    const auto image = m_image_reader->read_grayscale(image_path);
    Eigen::VectorXd pixels(static_cast<Eigen::Index>(image.pixels.size()));

    for (Eigen::Index index = 0; index < pixels.size(); ++index) {
        pixels(index) = static_cast<double>(image.pixels[static_cast<std::size_t>(index)]) / 255.0;
    }

    return project(pixels);
}

RecognitionResult RecognitionService::recognize(const Eigen::VectorXd& pixels) const 
{
    const auto query_embedding = project(pixels);
    const auto maybe_match = m_matcher.match(query_embedding, m_repository->find_all());

    if (!maybe_match.has_value()) {
        return {};
    }

    const auto& [sample, distance] = *maybe_match;
    return RecognitionResult {
        .matched = true,
        .sample_id = sample.id,
        .subject = sample.subject,
        .label = sample.label,
        .distance = distance
    };
}

RecognitionResult RecognitionService::recognize_image(const std::filesystem::path& image_path) const 
{
    const auto image = m_image_reader->read_grayscale(image_path);
    Eigen::VectorXd pixels(static_cast<Eigen::Index>(image.pixels.size()));

    for (Eigen::Index index = 0; index < pixels.size(); ++index) {
        pixels(index) = static_cast<double>(image.pixels[static_cast<std::size_t>(index)]) / 255.0;
    }

    return recognize(pixels);
}

std::optional<FaceSample> RecognitionService::find_by_id(const std::string& id) const 
{
    return m_repository->find_by_id(id);
}

std::size_t RecognitionService::registered_faces() const 
{
    return m_repository->find_all().size();
}

bool RecognitionService::is_trained() const noexcept 
{
    return m_pca_model.is_trained();
}

const PcaModel& RecognitionService::model() const noexcept 
{
    return m_pca_model;
}

Eigen::MatrixXd RecognitionService::build_training_matrix(const std::vector<FaceSample>& samples) const 
{
    if (samples.empty()) {
        throw std::invalid_argument("samples must not be empty");
    }

    const auto feature_count = samples.front().pixels.size();
    if (feature_count == 0) {
        throw std::invalid_argument("samples must contain pixel data");
    }

   Eigen::MatrixXd training_matrix(static_cast<Eigen::Index>(samples.size()), feature_count);

    for (std::size_t index = 0; index < samples.size(); ++index) {
        if (samples[index].pixels.size() != feature_count) {
            throw std::invalid_argument("all samples must have the same dimensionality");
        }

        training_matrix.row(static_cast<Eigen::Index>(index)) = samples[index].pixels.transpose();
    }

    return training_matrix;
}


