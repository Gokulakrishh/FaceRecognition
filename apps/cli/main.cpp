#include "application/recognition_service.hpp"
#include "imageDecoder/image_folder_dataset_loader.hpp"
#include "imageDecoder/delegating_image_reader.hpp"
#include "imageDecoder/jpeg_image_reader.hpp"
#include "imageDecoder/png_image_reader.hpp"
#include "imageDecoder/pgm_image_reader.hpp"
#include "core/in_memory_face_repository.hpp"



#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
    

    if (argc < 3) {
        std::cerr << "usage: face_recognition_tool <dataset_dir> <query_image> [components]\n";
        return 1;
    }

/**
 * components means how many principal components you keep after PCA.

  In Eigenfaces:

  - each face image is a high-dimensional vector
  - PCA reduces that high-dimensional data into a smaller feature space
  - those reduced axes are the “principal components” or “eigenfaces”

  So if an image has, for example:

  - 10000 pixel values

  PCA can reduce it to:

  - 16
  - 32
  - 64
    
*/
      std::size_t components = 64;
  if (argc >= 4) {
      try {
          components = std::stoul(argv[3]);
          if (components == 0) {
              std::cerr << "components must be greater than 0\n";
              return 1;
          }
      } catch (const std::exception&) {
          std::cerr << "invalid components value: " << argv[3] << '\n';
          return 1;
      }
  }
    auto repository = std::make_shared<InMemoryFaceRepository>();
    auto image_reader = std::make_shared<DelegatingImageReader>(std::vector<std::shared_ptr<ImageReader>> {
                                                                                                            std::make_shared<PgmImageReader>(),
                                                                                                            std::make_shared<JpegImageReader>(),
                                                                                                            std::make_shared<PngImageReader>()});
    auto dataset_loader = std::make_shared<ImageFolderDatasetLoader>(image_reader);
    RecognitionService service(repository, dataset_loader, image_reader);

    service.train_from_folder(argv[1], components);
    const auto result = service.recognize_image(argv[2]);

    std::cout << "face_recognition_tool\n";
    std::cout << "registered_faces=" << service.registered_faces() << '\n';
    std::cout << "components=" << service.model().components() << '\n';
    std::cout << "matched=" << (result.matched ? "true" : "false") << '\n';
    if (result.matched) {
        std::cout << "subject=" << result.subject << '\n';
        std::cout << "sample_id=" << result.sample_id << '\n';
        std::cout << "distance=" << result.distance << '\n';
    }
    

    return 0;
}
