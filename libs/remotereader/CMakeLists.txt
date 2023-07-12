set(LIBNAME remotereader)
add_library(${LIBNAME} remotereader.cpp)

find_package(Boost REQUIRED COMPONENTS headers)

target_link_directories(
    ${LIBNAME} PUBLIC
)

target_link_libraries(
    ${LIBNAME} PUBLIC 
    Boost::headers
)