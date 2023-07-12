cmake_minimum_required(VERSION 3.10)
project(ffmpeg_rtmp LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FFMPEG_PREFIX_PATH "/usr/local/Cellar/ffmpeg/6.0/")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_subdirectory(libs/listdevice)
add_subdirectory(libs/videodevice)
add_subdirectory(libs/videoencoder)
add_subdirectory(libs/audioencoder)
add_subdirectory(libs/audiodevice)
add_subdirectory(libs/audioresampler)
add_subdirectory(libs/rtmpwriter)
add_subdirectory(libs/videorescaler)
add_subdirectory(libs/timer)
add_subdirectory(libs/filereader)
add_subdirectory(libs/remotereader)
add_subdirectory(libs/imgoverlay)
add_subdirectory(libs/txtoverlay)

set(
    CPP_SOURCE_FILES
    main.cpp
)

add_executable(${PROJECT_NAME} ${CPP_SOURCE_FILES})

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
    libs/listdevice
    libs/videodevice
    libs/videoencoder
    libs/audioencoder
    libs/audiodevice
    libs/audioresampler
    libs/rtmpwriter
    libs/videorescaler
    libs/timer
    libs/filereader
    libs/remotereader
    libs/imgoverlay
    libs/txtoverlay
    )
    
target_link_directories(
    ${PROJECT_NAME} PUBLIC
    libs/listdevice
    libs/videodevice
    libs/videoencoder
    libs/audioencoder
    libs/audiodevice
    libs/audioresampler
    libs/rtmpwriter
    libs/videorescaler
    libs/timer
    libs/filereader
    libs/remotereader
    libs/imgoverlay
    libs/txtoverlay
)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC 
    listdevice
    videodevice
    videoencoder
    audioencoder
    audiodevice
    audioresampler
    rtmpwriter
    videorescaler
    timer
    filereader
    remotereader
    imgoverlay
    txtoverlay
)