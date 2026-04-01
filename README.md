# FaceRecognition

Lightweight face recognition project in modern C++20 using an Eigenfaces/PCA pipeline, a CLI, and a REST API.

Author: Gokulakrishnan Sivakumar

## Purpose

This project is a compact face recognition engine intended for learning, experimentation, and extension.

It includes:

- PCA-based Eigenfaces training
- face projection into eigen space
- nearest-neighbor matching
- support for PGM, JPEG, and PNG images
- multithreaded folder loading to speed up dataset ingestion
- REST API for training, status, and recognition

## Tech Stack

- Modern C++20
- Eigen for linear algebra
- `std::async`-based multithreading for parallel image loading
- CMake for builds

## Project Structure

- `include/`: public headers
- `src/`: implementation files
- `apps/cli/`: command-line entry point
- `apps/api/`: REST API server entry point
- `interfaces/rest/`: REST controller and HTTP server layer

Main components:

- `PcaModel`: trains the PCA basis and projects faces
- `RecognitionService`: orchestrates training and recognition
- `DelegatingImageReader`: dispatches image decoding by file type
- `ImageFolderDatasetLoader`: scans a dataset folder and loads images in parallel

## Multithreading

Dataset loading is parallelized at the image level.

That means:

- the dataset folder is scanned first
- supported images are collected
- images are decoded and normalized concurrently
- results are merged back into the final training sample list

This improves throughput when the dataset contains many images.

## Supported Image Formats

- `.pgm`
- `.jpg`
- `.jpeg`
- `.png`

## Build

From the project root:

```bash
mkdir -p build
cd build
cmake ..
make -j
```

Or with an out-of-tree build:

```bash
cmake -S . -B build
cmake --build build -j
```

## Run CLI

The CLI executable is:

```bash
./build/face_recognition_tool
```

Usage:

```bash
./build/face_recognition_tool <dataset_dir> <query_image> [components]
```

Example:

```bash
./build/face_recognition_tool ./dataset ./dataset/query.png 64
```

Arguments:

- `dataset_dir`: root folder containing subject subfolders
- `query_image`: image to recognize
- `components`: optional PCA component count, default is set in the CLI as 64

## Run REST API

The API executable is:

```bash
./build/face_recognition_api
```

Start it on the default port:

```bash
./build/face_recognition_api
```

Or specify a port:

```bash
./build/face_recognition_api 8080
```

Available endpoints:

- `GET /health`
- `GET /api/v1/status`
- `POST /api/v1/train`
- `POST /api/v1/recognize`

Train request:

```bash
curl -X POST http://localhost:8080/api/v1/train \
  -H "Content-Type: application/json" \
  -d '{"dataset_path":"./dataset","components":64}'
```

Recognize request:

```bash
curl -X POST http://localhost:8080/api/v1/recognize \
  -H "Content-Type: application/json" \
  -d '{"image_path":"./dataset/query.png"}'
```

## Dataset Layout

Images should be organized by subject folder.

Example layout:

```text
dataset/
  person_1/
    1.png
    2.jpg
  person_2/
    1.pgm
    2.png
  query.png
```

You can add your own images later under this structure.

## Notes

- all training images should have the same dimensions
- query images should match the training image dimensions
- if system `libjpeg` or `libpng` are unavailable, build behavior depends on the current decoder setup in the source tree
