cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(GravSim
        VERSION 0.0.10
        LANGUAGES CXX)

configure_file(
        "${GravSim_SOURCE_DIR}/include/GravSim/meta.hh.in"
        "${PROJECT_BINARY_DIR}/include/GravSim/meta.hh"
)

file(COPY assets DESTINATION ${PROJECT_BINARY_DIR})

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_BINARY_DIR}/include/*.hh" "${PROJECT_SOURCE_DIR}/include/*.hh")

list(REMOVE_ITEM HEADER_LIST "${PROJECT_SOURCE_DIR}/include/GravSim/meta.hh")


set(SOURCES
        src/main.cc
        src/logging.cc
        src/window.cc
        src/shader.cc
        src/vertex_object.cc
        src/timer.cc
        src/line.cc
        
        include/GravSim/utils.hh
)

add_compile_definitions(GravSim PUBLIC GLM_ENABLE_EXPERIMENTAL)

find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


add_executable(GravSim ${SOURCES} ${HEADER_LIST})

target_compile_features(GravSim PRIVATE cxx_std_23)
target_include_directories(GravSim PUBLIC ${PROJECT_BINARY_DIR}/include ${PROJECT_SOURCE_DIR}/include/)
target_link_libraries(GravSim PUBLIC spdlog::spdlog)
target_link_libraries(GravSim PRIVATE glfw)
target_link_libraries(GravSim PRIVATE glad::glad)
target_link_libraries(GravSim PRIVATE glm::glm)
target_link_libraries(GravSim PRIVATE imgui::imgui)
