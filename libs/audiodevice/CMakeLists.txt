set(LIBNAME audiodevice)
add_library(${LIBNAME} audiodevice.cpp)

target_include_directories(
    ${LIBNAME}
    PUBLIC 
    ${FFMPEG_PREFIX_PATH}/include/
)

target_link_directories(
    ${LIBNAME} PUBLIC
    ${FFMPEG_PREFIX_PATH}/lib/
)

target_link_libraries(
    ${LIBNAME} PUBLIC 
    libavutil.a 
    libavdevice.a 
    libavcodec.a
    libavformat.a
)