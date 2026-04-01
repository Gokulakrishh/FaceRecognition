// Author: Gokulakrishnan Sivakumar

#include "interfaces/rest/rest_server.hpp"
#include "core/helper.hpp"

#include <array>
#include <cstring>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


RestServer::RestServer(std::shared_ptr<RestController> controller, const std::uint16_t port)
    : m_controller(std::move(controller)),
      m_port(port)
{
    if (!m_controller) {
        throw std::invalid_argument("rest server requires a controller");
    }
}

void RestServer::run() const
{
    const int server_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        throw std::runtime_error("unable to create server socket");
    }

    int reuse = 1;
    ::setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

    if (::bind(server_socket, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) < 0) {
        ::close(server_socket);
        throw std::runtime_error("unable to bind server socket");
    }

    if (::listen(server_socket, 16) < 0) {
        ::close(server_socket);
        throw std::runtime_error("unable to listen on server socket");
    }

    std::cout << "face_recognition_api listening on port " << m_port << '\n';

    while (true) {
        const int client_socket = ::accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            continue;
        }

        try {
            const auto request = read_request(client_socket);
            write_response(client_socket, route(request));
        } catch (const std::exception& exception) {
            write_response(client_socket, Response {
                .status_code = 400,
                .body = std::string{"{\"error\":\""} + exception.what() + "\"}"
            });
        }

        ::close(client_socket);
    }
}

RestServer::Request RestServer::read_request(const int client_socket) const
{
    std::string raw;
    std::array<char, 4096> buffer {};

    while (raw.find("\r\n\r\n") == std::string::npos) {
        const auto bytes_read = ::recv(client_socket, buffer.data(), buffer.size(), 0);
        if (bytes_read <= 0) {
            throw std::runtime_error("unable to read request");
        }

        raw.append(buffer.data(), static_cast<std::size_t>(bytes_read));
    }

    const auto header_end = raw.find("\r\n\r\n");
    const auto header_block = raw.substr(0, header_end);
    std::istringstream stream(header_block);

    Request request;
    stream >> request.method >> request.path;

    std::size_t content_length = 0;
    std::string line;
    std::getline(stream, line);
    while (std::getline(stream, line)) {
        line = helper::trim(line);
        if (line.rfind("Content-Length:", 0) == 0) {
            content_length = static_cast<std::size_t>(std::stoul(helper::trim(line.substr(std::strlen("Content-Length:")))));
        }
    }

    request.body = raw.substr(header_end + 4);
    while (request.body.size() < content_length) {
        const auto bytes_read = ::recv(client_socket, buffer.data(), buffer.size(), 0);
        if (bytes_read <= 0) {
            throw std::runtime_error("unexpected end of request body");
        }

        request.body.append(buffer.data(), static_cast<std::size_t>(bytes_read));
    }

    if (request.body.size() > content_length) {
        request.body.resize(content_length);
    }

    return request;
}

RestServer::Response RestServer::route(const Request& request) const
{
    if (request.method == "GET" && request.path == "/health") {
        return Response {.status_code = 200, .body = m_controller->health()};
    }

    if (request.method == "GET" && request.path == "/api/v1/status") {
        return Response {.status_code = 200, .body = m_controller->status()};
    }

    if (request.method == "POST" && request.path == "/api/v1/train") {
        return Response {
            .status_code = 200,
            .body = m_controller->train(
                extract_json_string(request.body, "dataset_path"),
                extract_json_number(request.body, "components", 64)
            )
        };
    }

    if (request.method == "POST" && request.path == "/api/v1/recognize") {
        return Response {
            .status_code = 200,
            .body = m_controller->recognize(extract_json_string(request.body, "image_path"))
        };
    }

    return Response {.status_code = 404, .body = R"({"error":"route not found"})"};
}

void RestServer::write_response(const int client_socket, const Response& response)
{
    const std::string headers =
        "HTTP/1.1 " + std::to_string(response.status_code) + " " + reason_phrase(response.status_code) + "\r\n" +
        "Content-Type: application/json\r\n" +
        "Content-Length: " + std::to_string(response.body.size()) + "\r\n" +
        "Connection: close\r\n\r\n";

    const auto payload = headers + response.body;
    ::send(client_socket, payload.c_str(), payload.size(), 0);
}

std::string RestServer::extract_json_string(const std::string& body, const std::string& key)
{
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch match;
    if (!std::regex_search(body, match, pattern)) {
        throw std::invalid_argument("missing json string field: " + key);
    }

    return match[1].str();
}

std::size_t RestServer::extract_json_number(
    const std::string& body,
    const std::string& key,
    const std::size_t default_value
)
{
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(\\d+)");
    std::smatch match;
    if (!std::regex_search(body, match, pattern)) {
        return default_value;
    }

    return static_cast<std::size_t>(std::stoull(match[1].str()));
}

std::string RestServer::reason_phrase(const int status_code)
{
    switch (status_code) {
    case 200:
        return "OK";
    case 400:
        return "Bad Request";
    case 404:
        return "Not Found";
    default:
        return "Internal Server Error";
    }
}
