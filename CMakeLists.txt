cmake_minimum_required(VERSION 3.8)
project(ggcapture VERSION 1.0)
find_package(X11 REQUIRED)

option(build_example "build examples?" ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(deps/screen_capture_lite)
include_directories(include)

file(GLOB SOURCES src/*.cc)
file(GLOB HEADERS include/*.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES})


install (TARGETS ${PROJECT_NAME}  
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC
)
target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
        deps/screen_capture_lite/include
        deps/CImg-2.5.6
)
target_link_libraries(
    ${PROJECT_NAME} 
    screen_capture_lite 
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if(WIN32)
	target_link_libraries(
		${PROJECT_NAME} 
		dwmapi 
	)
endif(WIN32)

if(APPLE)
    target_link_directories(
        ${PROJECT_NAME}
        PUBLIC
            ${X11_DIR}
    )
    target_include_directories(
        ${PROJECT_NAME} 
        PUBLIC 
            deps/screen_capture_lite/include
            deps/CImg-2.5.6
            ${X11_INCLUDE_DIR}
    )
    target_link_libraries(
        ${PROJECT_NAME} 
        screen_capture_lite 
        ${X11_LIBRARIES}
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework ImageIO"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework AVFoundation"
        "-framework AppKit"
    )
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
endif(APPLE)

if(build_example)
	add_subdirectory(deps/gginput)
	add_subdirectory(example)
endif(build_example)
