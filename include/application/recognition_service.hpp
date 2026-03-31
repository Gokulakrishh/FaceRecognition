#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "application/face_dataset_loader.hpp"
#include "application/image_reader.hpp"
#include "application/recognition_result.hpp"
#include "core/nearest_neighbor_matcher.hpp"
#include "core/pca_model.hpp"
#include "model/face_repository.hpp"



class RecognitionService {
public:
    RecognitionService(
        std::shared_ptr<FaceRepository> repository,
        std::shared_ptr<FaceDatasetLoader> dataset_loader,
        std::shared_ptr<ImageReader> image_reader
    );

    void train(std::vector<FaceSample> samples, std::size_t components);
    void train_from_folder(const std::filesystem::path& dataset_root, std::size_t components);
    void register_sample(FaceSample sample);
    [[nodiscard]] Eigen::VectorXd project(const Eigen::VectorXd& pixels) const;
    [[nodiscard]] Eigen::VectorXd project_image(const std::filesystem::path& image_path) const;
    [[nodiscard]] RecognitionResult recognize(const Eigen::VectorXd& pixels) const;
    [[nodiscard]] RecognitionResult recognize_image(const std::filesystem::path& image_path) const;
    [[nodiscard]] std::optional<FaceSample> find_by_id(const std::string& id) const;
    [[nodiscard]] std::size_t registered_faces() const;
    [[nodiscard]] bool is_trained() const noexcept;
    [[nodiscard]] const PcaModel& model() const noexcept;

private:
    [[nodiscard]] Eigen::MatrixXd build_training_matrix(
        const std::vector<FaceSample>& samples
    ) const;

    PcaModel m_pca_model;
    NearestNeighborMatcher m_matcher;
    std::shared_ptr<FaceRepository> m_repository;
    std::shared_ptr<FaceDatasetLoader> m_dataset_loader;
    std::shared_ptr<ImageReader> m_image_reader;
};


