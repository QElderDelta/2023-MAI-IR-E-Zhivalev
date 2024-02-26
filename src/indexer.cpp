#include "indexer.h"

#include "lexer.h"
#include "tokenizer.h"
#include "utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

namespace fs = std::filesystem;

namespace {
    bool validateDirectory(std::string_view directory) {
        return fs::exists(directory) && fs::is_directory(directory);
    }

    std::vector<fs::path> listDirectory(std::string_view directory) {
        std::vector<fs::path> result;

        for (const auto& entry : fs::directory_iterator(directory)) {
            result.push_back(entry.path());
        }

        return result;
    }

    Index::IndexPerDocumentType processDocument(const fs::path& path) {
        Index::IndexPerDocumentType result;

        std::ifstream file_stream(path);

        std::string line;
        uint64_t current_pos = 0;
        while (std::getline(file_stream, line)) {
            for (const auto& term : Tokenizer::tokenize(line)) {
                if (auto processed_term = Lexer::processTerm(term); !processed_term.empty()) {
                    result[processed_term].entries.push_back(current_pos);
                    ++result[processed_term].entry_count;
                }

                ++current_pos;
            }
        }

        return result;
    }

    Index processFileBatch(const std::vector<fs::path>& files, uint64_t begin_idx, uint64_t end_idx) {
        Index result;

        for (auto idx = begin_idx; idx < end_idx; ++idx) {
            auto document_index = processDocument(files[idx]);

            if (!document_index.empty()) {
                result.addDocument(idx, {.header = files[idx].filename().string(), .link = files[idx].string()},
                                   std::move(document_index));
            }
        }

        return result;
    }
}

Index Indexer::indexDirectory(std::string_view directory, uint32_t processing_threads) {
    Timer timer("Indexing");

    if (!validateDirectory(directory)) {
        throw std::invalid_argument("Directory with provided name does not exist");
    }

    const auto filesInDirectory = listDirectory(directory);

    if (filesInDirectory.empty()) {
        return {};
    }

    if (processing_threads <= 1) {
        return processFileBatch(filesInDirectory, 0, filesInDirectory.size());
    }

    std::vector<Index> intermediate_results(processing_threads);
    std::vector<std::thread> threads;
    threads.reserve(processing_threads);

    uint64_t files_per_thread = (filesInDirectory.size() + processing_threads - 1) / processing_threads;
    for (uint32_t thread_id = 0; thread_id < processing_threads; ++thread_id) {
        auto begin = thread_id * files_per_thread;

        if (begin >= filesInDirectory.size()) {
            break;
        }

        auto end = std::min(begin + files_per_thread, filesInDirectory.size());

        threads.emplace_back([&filesInDirectory, &intermediate_results,thread_id, begin, end]() {
            intermediate_results[thread_id] = processFileBatch(filesInDirectory, begin, end);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (int idx = 1; idx < processing_threads; ++idx) {
        intermediate_results[0].merge(std::move(intermediate_results[idx]));
    }

    return intermediate_results[0];
}
