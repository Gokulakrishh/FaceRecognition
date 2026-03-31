#pragma once

#include <eigen3/Eigen/Dense>

class PcaModel {
public:
    void fit(const Eigen::MatrixXd& training_matrix, std::size_t components);
    [[nodiscard]] Eigen::VectorXd project(const Eigen::VectorXd& input) const;
    [[nodiscard]] Eigen::MatrixXd project(const Eigen::MatrixXd& input) const;

    [[nodiscard]] bool is_trained() const noexcept;
    [[nodiscard]] const Eigen::VectorXd& mean() const noexcept;
    [[nodiscard]] const Eigen::MatrixXd& basis() const noexcept;
    [[nodiscard]] std::size_t components() const noexcept;

private:
    Eigen::VectorXd m_mean;
    Eigen::MatrixXd m_basis;
    bool m_trained {false};
};


