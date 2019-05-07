cmake_minimum_required(VERSION 3.8)
project(ggcapture_example)
find_package(X11 REQUIRED)

add_executable(${PROJECT_NAME} main.cc)
target_link_directories(
    ${PROJECT_NAME}
    PUBLIC
)
target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
)
target_link_libraries(
    ${PROJECT_NAME} 
	ggcapture
	gginput
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

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
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
endif(APPLE)

