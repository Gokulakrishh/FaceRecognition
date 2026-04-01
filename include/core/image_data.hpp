#pragma once

#include <cstddef>
#include <vector>

struct ImageData {
    std::size_t width {0};
    std::size_t height {0};
    std::vector<unsigned char> pixels;
};

