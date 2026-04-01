#include "core/pca_model.hpp"

#include <algorithm>
#include <stdexcept>


void PcaModel::fit(const Eigen::MatrixXd& training_matrix, const std::size_t components) 
{
    if (training_matrix.rows() == 0 || training_matrix.cols() == 0) {
        throw std::invalid_argument("training_matrix must not be empty");
    }

    m_mean = training_matrix.colwise().mean();
    const auto centered = training_matrix.rowwise() - m_mean.transpose();

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(centered, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const auto safe_components = std::min<std::size_t>(components, static_cast<std::size_t>(svd.matrixV().cols()));
    m_basis = svd.matrixV().leftCols(static_cast<Eigen::Index>(safe_components));
    m_trained = safe_components > 0;
}

Eigen::VectorXd PcaModel::project(const Eigen::VectorXd& input) const 
{
    if (!m_trained) {
        throw std::logic_error("pca model is not trained");
    }

    if (input.size() != m_mean.size()) {
        throw std::invalid_argument("input size does not match model mean size");
    }

    return m_basis.transpose() * (input - m_mean);
}

Eigen::MatrixXd PcaModel::project(const Eigen::MatrixXd& input) const 
{
    if (!m_trained) {
        throw std::logic_error("pca model is not trained");
    }

    if (input.cols() != m_mean.size()) {
        throw std::invalid_argument("input column count does not match model mean size");
    }

    const auto centered = input.rowwise() - m_mean.transpose();
    return centered * m_basis;
}

bool PcaModel::is_trained() const noexcept 
{
    return m_trained;
}

const Eigen::VectorXd& PcaModel::mean() const noexcept 
{
    return m_mean;
}

const Eigen::MatrixXd& PcaModel::basis() const noexcept 
{
    return m_basis;
}

std::size_t PcaModel::components() const noexcept 
{
    return static_cast<std::size_t>(m_basis.cols());
}


