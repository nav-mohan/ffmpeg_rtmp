set(LIBNAME timer)
add_library(${LIBNAME} timer.cpp)

find_package(Boost REQUIRED COMPONENTS headers)

target_link_directories(
    ${LIBNAME} PUBLIC
)

target_link_libraries(
    ${LIBNAME} PUBLIC 
    Boost::headers
)