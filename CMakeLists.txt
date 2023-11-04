
cmake_minimum_required(VERSION 3.23)
project(svgmorph)

add_executable(
    svgmorph
    src/main.cpp
    )

target_compile_features(
    svgmorph
    PRIVATE
    cxx_std_20
    )

find_package(Threads)
target_link_libraries(
    svgmorph
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    )

file(
    COPY
    data
    DESTINATION
    .
    )
