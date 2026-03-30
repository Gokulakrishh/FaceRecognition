#include <iostream>
#include <memory>
#include <string>



int main(int argc, char** argv) {
    

    if (argc < 3) {
        std::cerr << "usage: face_recognition_tool <dataset_dir> <query_image> [components]\n";
        return 1;
    }

    

    std::cout << "face_recognition_tool\n";
    std::cout << "registered_faces="  << '\n';
    std::cout << "components="  << '\n';
    std::cout << "matched="  << '\n';
    

    return 0;
}
