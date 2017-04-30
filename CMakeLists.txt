cmake_minimum_required(VERSION 2.8)

set(BIN html_parser)
project(${BIN})

set(HEADERS_DIR ./)
set(SOURCES_DIR ./)

aux_source_directory(${SOURCES_DIR} SOURCES_LIST)
file(GLOB_RECURSE HEADERS_LIST "./*.h")

# deps
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.59.0 COMPONENTS regex)

include_directories(${HEADERS_DIR} ${HEADERS_LIST} ${Boost_INCLUDE_DIRS})
add_library(${BIN} STATIC ${SOURCES_LIST} ${HEADERS_LIST})
target_link_libraries(${BIN} ${Boost_LIBRARIES})