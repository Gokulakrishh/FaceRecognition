// Author: Gokulakrishnan Sivakumar

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "interfaces/rest/rest_controller.hpp"

class RestServer {
public:
    RestServer(std::shared_ptr<RestController> controller, std::uint16_t port);

    void run() const;

private:
    struct Request {
        std::string method;
        std::string path;
        std::string body;
    };

    struct Response {
        int status_code {200};
        std::string body;
    };

    [[nodiscard]] Request read_request(int client_socket) const;
    [[nodiscard]] Response route(const Request& request) const;
    static void write_response(int client_socket, const Response& response);
    [[nodiscard]] static std::string extract_json_string(const std::string& body, const std::string& key);
    [[nodiscard]] static std::size_t extract_json_number(const std::string& body, const std::string& key, std::size_t default_value);
    [[nodiscard]] static std::string reason_phrase(int status_code);

    std::shared_ptr<RestController> m_controller;
    std::uint16_t m_port;
};
