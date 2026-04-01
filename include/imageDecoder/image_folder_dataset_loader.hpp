#pragma once

#include <memory>

#include "model/face_dataset_loader.hpp"
#include "imageDecoder/image_reader.hpp"



class ImageFolderDatasetLoader final : public FaceDatasetLoader {
public:
    explicit ImageFolderDatasetLoader(std::shared_ptr<ImageReader> image_reader);

    [[nodiscard]] std::vector<FaceSample> load_from_folder(const std::filesystem::path& root) const override;

private:
    [[nodiscard]] Eigen::VectorXd normalize(const ImageData& image) const;

    std::shared_ptr<ImageReader> m_image_reader;
};

