cmake_minimum_required(VERSION 3.22)
project(Airwave VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

find_package(PkgConfig QUIET)
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)
find_library(AVUTIL_LIBRARY avutil)
find_library(SWSCALE_LIBRARY swscale)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/protocol
    ${AVCODEC_INCLUDE_DIR}
)

set(SOURCES
    src/main.cpp
    src/protocol/AwtpReceiver.cpp
    src/video/HwDecoder.cpp
    src/video/VideoRendererItem.cpp
    src/stats/LatencyTracker.cpp
)

set(HEADERS
    src/protocol/AwtpReceiver.hpp
    src/video/HwDecoder.hpp
    src/video/VideoRendererItem.hpp
    src/stats/LatencyTracker.hpp
)

qt_add_resources(RESOURCES
    qml.qrc
)

add_executable(Airwave
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_libraries(Airwave PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    ${AVCODEC_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${SWSCALE_LIBRARY}
)

if (WIN32)
    target_link_libraries(Airwave PRIVATE d3d11 dxgi)
endif()
