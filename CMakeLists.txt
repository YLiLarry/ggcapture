cmake_minimum_required(VERSION 3.8)
project(ggcapture VERSION 1.0)

option(ggcapture_build_example "build examples?" ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(BUILD_EXAMPLE OFF) # disable example for screen_capture_lite
add_subdirectory(deps/screen_capture_lite)
add_subdirectory(deps/ggframe)

include_directories(include)

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
        deps/screen_capture_lite/include
		deps/ggframe/include
)
target_link_libraries(
    ${PROJECT_NAME} 
    screen_capture_lite 
	ggframe
)

if(WIN32)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
            WIN32=1
    )
	target_link_libraries(
		${PROJECT_NAME} 
		dwmapi 
	)
endif(WIN32)

if(APPLE)
    find_package(X11 REQUIRED)
    find_package(Boost REQUIRED)
    # macOS still does not ship with C++17 filesystem header
    # we need to use boost::filesystem instead
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
            APPLE=1
    )
    target_link_directories(
        ${PROJECT_NAME}
        PUBLIC
            ${X11_LIBRARY_DIRS}
            ${Boost_LIBRARY_DIRS}
    )
    target_include_directories(
        ${PROJECT_NAME} 
        PUBLIC 
            ${X11_INCLUDE_DIR}
            ${Boost_INCLUDE_DIR}
    )
    target_link_libraries(
        ${PROJECT_NAME} 
        screen_capture_lite 
        ${X11_LIBRARIES}
        boost_filesystem
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework ImageIO"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework AVFoundation"
        "-framework AppKit"
    )
endif(APPLE)

if(ggcapture_build_example)
	add_subdirectory(example)
endif(ggcapture_build_example)
