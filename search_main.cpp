#include "engine.h"

#include <iostream>

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cout << "USAGE: ./search_engine <path_to_index_dump>" << std::endl;
        return EXIT_FAILURE;
    }

    SearchEngine engine(argv[1]);

    std::string query;
    while (std::getline(std::cin, query)) {
        const auto result = engine.processSearchRequest(query);
        std::cout << "Document count: " << result.size() << '\n';

        for (const auto& document : result) {
            std::cout << document << '\n';
        }

        std::cout << "----------------------------" << '\n';
    }

    return EXIT_SUCCESS;
}
