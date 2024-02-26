#include "indexer.h"

#include <fstream>
#include <iostream>

int main(int argc, const char* argv[]) {
    if (argc != 4) {
        std::cout << "USAGE: ./indexer <path_to_dir_with_docs> <path_to_result> <processing_threads>" << std::endl;
        return EXIT_FAILURE;
    }

    const char* docs_directory = argv[1];
    const char* result_file = argv[2];
    const int processing_threads = std::atoi(argv[3]);

    std::ofstream result_stream(result_file, std::ios::binary);
    Indexer::indexDirectory(docs_directory, processing_threads).serializeBinary(result_stream);

    return EXIT_SUCCESS;
}
