#pragma once

#include <optional>
#include <vector>

#include "model/face_sample.hpp"


class NearestNeighborMatcher {
public:
    [[nodiscard]] std::optional<std::pair<FaceSample, double>> match(
        const Eigen::VectorXd& query_embedding,
        const std::vector<FaceSample>& gallery
    ) const;
};


