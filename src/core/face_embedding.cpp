#include "core/face_embedding.hpp"



FaceEmbedding::FaceEmbedding(const PcaModel& model)
    : m_model(model) 
{

}

Eigen::VectorXd FaceEmbedding::encode(const Eigen::VectorXd& pixels) const 
{
    return m_model.project(pixels);
}


