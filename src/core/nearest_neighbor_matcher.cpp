#include "face_engine/core/eigenfaces/nearest_neighbor_matcher.hpp"

#include <limits>
#include <stdexcept>

namespace face_engine::core::eigenfaces {

std::optional<std::pair<domain::entities::FaceSample, common::Distance>> NearestNeighborMatcher::match(
    const common::FeatureVector& query_embedding,
    const std::vector<domain::entities::FaceSample>& gallery
) const {
    if (gallery.empty()) {
        return std::nullopt;
    }

    common::Distance best_distance = std::numeric_limits<common::Distance>::max();
    const domain::entities::FaceSample* best_match = nullptr;

    for (const auto& candidate : gallery) {
        if (candidate.embedding.size() == 0) {
            continue;
        }

        if (candidate.embedding.size() != query_embedding.size()) {
            throw std::invalid_argument("gallery embedding dimension does not match query embedding dimension");
        }

        const auto distance = (candidate.embedding - query_embedding).norm();
        if (distance < best_distance) {
            best_distance = distance;
            best_match = &candidate;
        }
    }

    if (best_match == nullptr) {
        return std::nullopt;
    }

    return std::make_pair(*best_match, best_distance);
}

}  // namespace face_engine::core::eigenfaces
