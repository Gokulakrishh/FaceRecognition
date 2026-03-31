#pragma once

#include <optional>
#include <vector>

#include "model/face_sample.hpp"


class FaceRepository {
public:
    virtual ~FaceRepository() = default;

    virtual void save(FaceSample sample) = 0;
    virtual std::vector<FaceSample> find_all() const = 0;
    virtual std::optional<FaceSample> find_by_id(const std::string& id) const = 0;
    virtual void clear() = 0;
};

