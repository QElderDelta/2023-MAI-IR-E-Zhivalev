#include "lexer.h"

std::string Lexer::processTerm(std::string_view term) {
    auto word_begin = std::find_if(term.begin(), term.end(), [](char ch) { return isalpha(ch); });

    if (word_begin == term.end()) {
        return {};
    }

    auto word_end = std::find_if(term.rbegin(), term.rend(), [](char ch) { return isalpha(ch); });

    auto begin_idx = std::distance(term.begin(), word_begin);
    auto end_idx = term.size() - std::distance(term.rbegin(), word_end);

    std::string result;
    result.reserve(end_idx - begin_idx);

    for (auto idx = begin_idx; idx < end_idx; ++idx) {
        if (!isalpha(term[idx])) {
            return {};
        }

        result.push_back(tolower(term[idx]));
    }

    return result;
}

