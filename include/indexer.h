#pragma once

#include "index.h"

#include <string_view>

class Indexer {
public:
    static Index indexDirectory(std::string_view directory, uint32_t processing_threads = 1);
};