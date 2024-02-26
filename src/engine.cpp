#include "engine.h"

#include "lexer.h"
#include "tokenizer.h"
#include "utils.h"

#include <fstream>

namespace {
    std::vector<uint64_t> intersectDocumentLists(const std::vector<uint64_t>& lhs,
                                                 const std::vector<uint64_t>& rhs) {
        std::vector<uint64_t> result;

        uint64_t lhs_pos = 0;
        uint64_t rhs_pos = 0;

        while (lhs_pos < lhs.size() && rhs_pos < rhs.size()) {
            if (lhs[lhs_pos] == rhs[rhs_pos]) {
                result.push_back(lhs[lhs_pos]);
                ++lhs_pos;
                ++rhs_pos;
            } else if (lhs[lhs_pos] < rhs[rhs_pos]) {
                ++lhs_pos;
            } else {
                ++rhs_pos;
            }
        }

        return result;
    }
}

SearchEngine::SearchEngine(const char* index_dump_path) {
    std::ifstream stream(index_dump_path, std::ios::binary);
    index_.deserializeBinary(stream);
}

std::vector<std::string> SearchEngine::processSearchRequest(const std::string& request) {
    Timer timer("Request processing");

    std::optional<std::vector<uint64_t>> resulting_document_list;

    for (const auto& term : Tokenizer::tokenize(request)) {
        if (const auto processed_term = Lexer::processTerm(term); !term.empty()) {
            if (resulting_document_list.has_value()) {
                resulting_document_list = intersectDocumentLists(*resulting_document_list,
                                                                 index_.query(processed_term));
            } else {
                resulting_document_list = index_.query(processed_term);
            }
        } else {
            return {};
        }
    }

    if (!resulting_document_list.has_value() || resulting_document_list->empty()) {
        return {};
    }

    std::vector<std::string> result;
    result.reserve(resulting_document_list->size());

    for (uint64_t doc_id : *resulting_document_list) {
        result.push_back(index_.getDocumentHeader(doc_id));
    }

    return result;
}

