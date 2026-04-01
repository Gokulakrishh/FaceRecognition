// Author: Gokulakrishnan Sivakumar

#pragma once

#include <eigen3/Eigen/Dense>
#include "core/pca_model.hpp"


class FaceEmbedding {
public:
    explicit FaceEmbedding(const PcaModel& model);

    [[nodiscard]] Eigen::VectorXd encode(const Eigen::VectorXd& pixels) const;

private:
    const PcaModel& m_model;
};

