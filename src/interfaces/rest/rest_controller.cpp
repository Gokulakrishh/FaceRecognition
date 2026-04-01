// Author: Gokulakrishnan Sivakumar

#include "interfaces/rest/rest_controller.hpp"

#include <filesystem>
#include <stdexcept>
#include <utility>

RestController::RestController(std::shared_ptr<RecognitionService> recognition_service)
    : m_recognition_service(std::move(recognition_service))
{
    if (!m_recognition_service) {
        throw std::invalid_argument("rest controller requires a recognition service");
    }
}

std::string RestController::health() const
{
    return R"({"status":"ok","service":"face-recognition-api"})";
}

std::string RestController::status() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return std::string{"{\"trained\":"} + (m_recognition_service->is_trained() ? "true" : "false") +
           ",\"registered_faces\":" + std::to_string(m_recognition_service->registered_faces()) +
           ",\"components\":" + std::to_string(m_recognition_service->model().components()) + "}";
}

std::string RestController::train(const std::string& dataset_path, const std::size_t components)
{
    if (dataset_path.empty()) {
        throw std::invalid_argument("dataset_path must not be empty");
    }

    if (components == 0) {
        throw std::invalid_argument("components must be greater than 0");
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_recognition_service->train_from_folder(std::filesystem::path(dataset_path), components);

    return std::string{"{\"status\":\"trained\",\"dataset_path\":\""} + escape_json(dataset_path) +
           "\",\"registered_faces\":" + std::to_string(m_recognition_service->registered_faces()) +
           ",\"components\":" + std::to_string(m_recognition_service->model().components()) + "}";
}

std::string RestController::recognize(const std::string& image_path) const
{
    if (image_path.empty()) {
        throw std::invalid_argument("image_path must not be empty");
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    const auto result = m_recognition_service->recognize_image(std::filesystem::path(image_path));

    return std::string{"{\"matched\":"} + (result.matched ? "true" : "false") +
           ",\"image_path\":\"" + escape_json(image_path) +
           "\",\"subject\":\"" + escape_json(result.subject) +
           "\",\"sample_id\":\"" + escape_json(result.sample_id) +
           "\",\"label\":" + std::to_string(result.label) +
           ",\"distance\":" + std::to_string(result.distance) + "}";
}

std::string RestController::escape_json(const std::string& value)
{
    std::string escaped;
    escaped.reserve(value.size());

    for (const char ch : value) {
        switch (ch) {
        case '\\':
            escaped += "\\\\";
            break;
        case '"':
            escaped += "\\\"";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped += ch;
            break;
        }
    }

    return escaped;
}
