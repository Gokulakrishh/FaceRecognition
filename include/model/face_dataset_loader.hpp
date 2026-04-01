#pragma once

#include <filesystem>
#include <vector>

#include "model/face_sample.hpp"



class FaceDatasetLoader {
public:
    virtual ~FaceDatasetLoader() = default;

    [[nodiscard]] virtual std::vector<FaceSample> load_from_folder(const std::filesystem::path& root) const = 0;
};


