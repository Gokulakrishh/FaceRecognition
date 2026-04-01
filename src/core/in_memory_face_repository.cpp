#include "core/in_memory_face_repository.hpp"

#include <utility>



void InMemoryFaceRepository::save(FaceSample sample) 
{
    m_samples.insert_or_assign(sample.id, std::move(sample));
}

std::vector<FaceSample> InMemoryFaceRepository::find_all() const 
{
    std::vector<FaceSample> values;
    values.reserve(m_samples.size());

    for (const auto& [id, sample] : m_samples) {
        values.push_back(sample);
    }
      /*for (const auto& entry : m_samples) {
      const auto& sample = entry.second;
      values.push_back(sample);
  }*/

    return values;
}

std::optional<FaceSample> InMemoryFaceRepository::find_by_id(const std::string& id) const 
{
    const auto it = m_samples.find(id);
    if (it == m_samples.end()) {
        return std::nullopt;
    }

    return it->second;
}

void InMemoryFaceRepository::clear() 
{
    m_samples.clear();
}


