cmake_minimum_required(VERSION 3.26)
project(GravSim)
set(CMAKE_CXX_STANDARD 23)


file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_BINARY_DIR}/include/*.hh" "${PROJECT_SOURCE_DIR}/include/*.hh")
list(REMOVE_ITEM HEADER_LIST "${PROJECT_SOURCE_DIR}/include/GravSim/meta.hh")


set(SOURCES
    src/main.cpp
        src/logging.cc
        include/GravSim/logging.hh
)


find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)


add_executable(GravSim ${SOURCES} ${HEADER_LIST})

target_include_directories(GravSim PUBLIC ${PROJECT_BINARY_DIR}/include ${PROJECT_SOURCE_DIR}/include/)
target_link_libraries(GravSim PUBLIC glfw)
target_link_libraries(GravSim PUBLIC spdlog::spdlog)