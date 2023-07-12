set(LIBNAME imgoverlay)
add_library(${LIBNAME} imgoverlay.cpp)

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
    libavcodec.a
    libavformat.a
    libavfilter.a
    libswscale.a
)