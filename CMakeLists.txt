cmake_minimum_required(VERSION 3.17)

project(VSA)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -fsanitize=address,undefined -g -DNDEBUG")

find_package(FLEX)
flex_target(MyScanner generate/lex_analizer.lex  ${CMAKE_CURRENT_BINARY_DIR}/lex_analizer.cpp)

set(src ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/syntax_analysis_LR0.cpp
        ${CMAKE_SOURCE_DIR}/src/debug.cpp
        ${CMAKE_SOURCE_DIR}/src/tokens.cpp
        )

set(exec VSA.out)
add_executable(${exec}
    ${src}
    ${FLEX_MyScanner_OUTPUTS}
    )
target_compile_features(${exec} PUBLIC cxx_std_23)
target_include_directories(${exec} PUBLIC ${CMAKE_SOURCE_DIR}/include)

include(CTest)
enable_testing()

file(GLOB testfiles "${CMAKE_SOURCE_DIR}/tests/*.dat")
foreach(file ${testfiles})
    add_test (NAME ${file}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test.py ${CMAKE_SOURCE_DIR} ${file})
    set_tests_properties(${file} PROPERTIES DEPENDS ${exec})
endforeach()
