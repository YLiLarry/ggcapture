cmake_minimum_required(VERSION 3.8)
project(ggcapture_example)

add_executable(${PROJECT_NAME} ggcapture_example.cc)

target_link_libraries(
    ${PROJECT_NAME} 
    ggcapture
)
