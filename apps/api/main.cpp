// Author: Gokulakrishnan Sivakumar

#include "application/recognition_service.hpp"
#include "core/in_memory_face_repository.hpp"
#include "core/helper.hpp"
#include "imageDecoder/delegating_image_reader.hpp"
#include "imageDecoder/image_folder_dataset_loader.hpp"
#include "imageDecoder/jpeg_image_reader.hpp"
#include "imageDecoder/pgm_image_reader.hpp"
#include "imageDecoder/png_image_reader.hpp"
#include "interfaces/rest/rest_controller.hpp"
#include "interfaces/rest/rest_server.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>


int main(int argc, char** argv)
{
    //git CI/CD test
    try {
        auto repository = std::make_shared<InMemoryFaceRepository>();
        auto image_reader = std::make_shared<DelegatingImageReader>(std::vector<std::shared_ptr<ImageReader>> {
                                                                                                                std::make_shared<PgmImageReader>(),
                                                                                                                std::make_shared<JpegImageReader>(),
                                                                                                                std::make_shared<PngImageReader>()});
        auto dataset_loader = std::make_shared<ImageFolderDatasetLoader>(image_reader);
        auto recognition_service = std::make_shared<RecognitionService>(repository, dataset_loader, image_reader);
        auto controller = std::make_shared<RestController>(recognition_service);
        RestServer server(controller, helper::resolve_port(argc, argv));
        server.run();
    } catch (const std::exception& exception) {
        std::cerr << "face_recognition_api startup error: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
