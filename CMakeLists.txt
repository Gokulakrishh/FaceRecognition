cmake_minimum_required(VERSION 3.20)

project(
    face_recognition
    VERSION 0.1.0
    DESCRIPTION "Lightweight face recognition tool in Modern C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(PNG QUIET)
find_package(JPEG QUIET)

add_library(face_recognition
    src/application/recognition_service.cpp
    src/core/face_embedding.cpp
    src/core/in_memory_face_repository.cpp
    src/core/nearest_neighbor_matcher.cpp
    src/core/pca_model.cpp
    src/imageDecoder/delegating_image_reader.cpp
    src/imageDecoder/image_folder_dataset_loader.cpp
    src/imageDecoder/jpeg_image_reader.cpp
    src/imageDecoder/pgm_image_reader.cpp
    src/imageDecoder/png_image_reader.cpp
    src/imageDecoder/stb_image_decoder.cpp
)

target_include_directories(face_recognition PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(face_recognition PUBLIC cxx_std_20)
target_compile_options(
    face_recognition
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_link_libraries(face_recognition PUBLIC Eigen3::Eigen)

if(PNG_FOUND)
    target_link_libraries(face_recognition PUBLIC PNG::PNG)
endif()

if(JPEG_FOUND)
    target_link_libraries(face_recognition PUBLIC JPEG::JPEG)
endif()

add_executable(face_recognition_tool apps/cli/main.cpp)
target_link_libraries(face_recognition_tool PRIVATE face_recognition)
