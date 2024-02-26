cmake_minimum_required(VERSION 3.27)
project(2023_MAI_IR_E_Zhivalev)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests)

find_package(Threads)

add_executable(indexer
        include/index.h
        include/indexer.h
        include/lexer.h
        include/tokenizer.h
        include/utils.h
        src/index.cpp
        src/indexer.cpp
        src/lexer.cpp
        src/tokenizer.cpp
        src/utils.cpp
        indexer_main.cpp)
target_include_directories(indexer PRIVATE include)
target_link_libraries(indexer PRIVATE Threads::Threads)

add_executable(search_engine
        include/engine.h
        include/index.h
        include/lexer.h
        include/tokenizer.h
        include/utils.h
        src/engine.cpp
        src/index.cpp
        src/lexer.cpp
        src/tokenizer.cpp
        src/utils.cpp
        search_main.cpp)
target_include_directories(search_engine PRIVATE include)
