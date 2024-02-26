#pragma once

#include "index.h"

class SearchEngine {
public:
    SearchEngine(const char* index_dump_path);

    std::vector<std::string> processSearchRequest(const std::string& request);
private:
    Index index_;
};
