// Author: Gokulakrishnan Sivakumar

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <mutex>

#include "application/recognition_service.hpp"


class RestController {
public:
    explicit RestController(std::shared_ptr<RecognitionService> recognition_service);

    [[nodiscard]] std::string health() const;
    [[nodiscard]] std::string status() const;
    [[nodiscard]] std::string train(const std::string& dataset_path, std::size_t components);
    [[nodiscard]] std::string recognize(const std::string& image_path) const;

private:
    [[nodiscard]] static std::string escape_json(const std::string& value);

    std::shared_ptr<RecognitionService> m_recognition_service;
    mutable std::mutex m_mutex;
};
