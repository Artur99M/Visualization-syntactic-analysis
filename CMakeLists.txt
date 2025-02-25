# rm -rf build
# cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# cmake --build build
# for tests:
# cmake --build ./build --target test


cmake_minimum_required(VERSION 3.17)

project(VSA)
set(CMAKE_CXX_COMPILER "clang++")
# set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -g")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -fsanitize=address,undefined -g")

# find_package(FLEX)
# flex_target(MyScanner generate/lex_analizer.l  ${CMAKE_CURRENT_BINARY_DIR}/lex_analizer.cpp)

set(src ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/syntax_analysis_LR0.cpp
        ${CMAKE_SOURCE_DIR}/src/lex_analizer.cpp
        ${CMAKE_SOURCE_DIR}/src/debug.cpp
        ${CMAKE_SOURCE_DIR}/src/tokens.cpp
        )

set(exec VSA.out)
add_executable(${exec})
target_sources(${exec} PRIVATE ${src})
target_compile_features(${exec} PUBLIC cxx_std_17)
target_include_directories(${exec} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(${exec} PRIVATE
    $<$<CONFIG:Debug>:DEBUG=1>
)

include(CTest)
enable_testing()

file(GLOB testfiles "${CMAKE_SOURCE_DIR}/tests/*.dat")
foreach(file ${testfiles})
    add_test (NAME ${file}
        COMMAND ${CMAKE_SOURCE_DIR}/tests/test ${CMAKE_SOURCE_DIR} ${file})
    set_tests_properties(${file} PROPERTIES DEPENDS ${exec})
endforeach()
