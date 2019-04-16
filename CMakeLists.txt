cmake_minimum_required(VERSION 3.8)
project(ggcaputre)
find_package(X11 REQUIRED)

set(target a.out)
add_subdirectory(deps/screen_capture_lite)
add_executable(${target} src/main.cc)

if(APPLE)
    target_link_directories(
        ${target}
        PUBLIC
            ${X11_DIR}
    )
    target_include_directories(
        ${target} 
        PUBLIC 
            deps/screen_capture_lite/include
            deps/CImg-2.5.6
            ${X11_INCLUDE_DIR}
    )
    target_link_libraries(
        ${target} 
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
    target_compile_features(${target} PUBLIC cxx_std_11)
endif(APPLE)

