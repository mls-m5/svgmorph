
cmake_minimum_required(VERSION 3.23)
project(svgmorph)

find_package(Threads)

# =======================

add_executable(
    svgmorph
    src/main.cpp
    )

target_compile_features(
    svgmorph
    PRIVATE
    cxx_std_20
    )

target_link_libraries(
    svgmorph
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    )

# =======================

add_executable(
    showgraph
    src/showgraph.cpp
    )

target_compile_features(
    showgraph
    PRIVATE
    cxx_std_20
    )

#target_link_libraries(
#    svgmorph
#    PRIVATE
#    ${CMAKE_THREAD_LIBS_INIT}
#    )

# =========================

file(
    COPY
    data
    DESTINATION
    .
    )

