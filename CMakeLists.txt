cmake_minimum_required(VERSION 3.20)

project(
    face_recognition
    VERSION 0.1.0
    DESCRIPTION "Lightweight face recognition tool in C++20"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)



find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(PNG QUIET)
find_package(JPEG QUIET)




add_subdirectory(src)


