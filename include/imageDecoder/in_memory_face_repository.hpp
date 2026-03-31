#pragma once

#include <unordered_map>

#include "model/face_repository.hpp"



class InMemoryFaceRepository final : public FaceRepository {
public:
    void save(FaceSample sample) override;
    [[nodiscard]] std::vector<FaceSample> find_all() const override;
    [[nodiscard]] std::optional<FaceSample> find_by_id(const std::string& id) const override;
    void clear() override;

private:
    std::unordered_map<std::string, FaceSample> m_samples;
};


