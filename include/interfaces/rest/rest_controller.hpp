#pragma once

#include <string>


class RestController {
public:
    [[nodiscard]] std::string health() const;
};

