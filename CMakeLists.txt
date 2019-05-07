cmake_minimum_required(VERSION 3.8)
project(ggcapture VERSION 1.0)

option(build_ggcapture_example "build examples?" ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(BUILD_EXAMPLE OFF) # disable example for screen_capture_lite

add_subdirectory(deps/screen_capture_lite)
add_subdirectory(deps/ggframe)

file(GLOB SOURCES src/*.cc)
file(GLOB HEADERS include/*.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

# requires std=c++17 for filesystem support
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

install (TARGETS ${PROJECT_NAME}  
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
        include
        deps/screen_capture_lite/include
		deps/ggframe/include
)

target_link_libraries(
    ${PROJECT_NAME} 
    screen_capture_lite 
	ggframe
)

if(build_ggcapture_example)
	add_subdirectory(example)
endif(build_ggcapture_example)
