include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(tokenizer_test
        tokenizer_test.cpp
        ../src/tokenizer.cpp)
target_link_libraries(tokenizer_test PRIVATE GTest::GTest)
target_include_directories(tokenizer_test PRIVATE ../include)

add_test(tokenizer_test tokenizer_test)

add_executable(lexer_test
        lexer_test.cpp
        ../src/lexer.cpp)
target_link_libraries(lexer_test PRIVATE GTest::GTest)
target_include_directories(lexer_test PRIVATE ../include)

add_test(lexer_test lexer_test)