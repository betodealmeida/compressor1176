cmake_minimum_required(VERSION 3.10)
project(compressor1176)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LV2 REQUIRED lv2)

add_library(compressor1176 SHARED src/compressor1176.cpp)
target_include_directories(compressor1176 PRIVATE ${LV2_INCLUDE_DIRS})
target_link_libraries(compressor1176 PRIVATE ${LV2_LIBRARIES})

set_target_properties(compressor1176 PROPERTIES PREFIX "")

# Install to LV2 directory
install(TARGETS compressor1176
        LIBRARY DESTINATION lib/lv2/compressor1176.lv2)
install(FILES "${CMAKE_SOURCE_DIR}/lv2/manifest.ttl"
              "${CMAKE_SOURCE_DIR}/lv2/compressor1176.ttl"
        DESTINATION lib/lv2/compressor1176.lv2)