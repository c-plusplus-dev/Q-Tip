cmake_minimum_required(VERSION 4.0)

project(QTip
    VERSION 0.1.7
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

include(GNUInstallDirs)

add_library(QTip STATIC
        src/QTip.cpp
        include/Q-Tip/QTip.h
        include/Q-Tip/Config.h
        src/Window/Window.cpp
        include/Q-Tip/Window/Window.h
        src/Window/Input.cpp
        include/Q-Tip/Window/Input.h
        src/Graphics/Renderer.cpp
        include/Q-Tip/Graphics/Renderer.h
        include/Q-Tip/Graphics/Color.h
        include/Q-Tip/Graphics/Shapes/Rect.h
        src/Graphics/Font.cpp
        include/Q-Tip/Graphics/Font.h
        include/Q-Tip/Graphics/Texture.h
        src/Graphics/Texture.cpp
        include/Q-Tip/Graphics/Shapes/Circle.h
        include/Q-Tip/Math/Point.h
        include/Q-Tip/Graphics/Shapes/Triangle.h
        include/Q-Tip/Graphics/Shapes/Polygon.h
        include/Q-Tip/Clock.h
        src/Clock.cpp
        src/Graphics/Color.cpp
        src/Graphics/Shapes/Rect.cpp
        include/Q-Tip/Types/Lazy.h
        src/Graphics/RenderTexture.cpp
        include/Q-Tip/Graphics/RenderTexture.h
        include/Q-Tip/Mods/Moddable.h
        include/Q-Tip/Mods/Mod.h
        src/Mods/ModLoader/ModLoader.cpp
        include/Q-Tip/Mods/ModLoader/ModLoader.h
        include/Q-Tip/Logger/Log.h
)

add_library(QTip::QTip ALIAS QTip)

target_include_directories(QTip
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_link_libraries(QTip
    PRIVATE
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
        SDL3_image::SDL3_image
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/QTipConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QTipConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QTip
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QTipConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/QTipConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/QTipConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QTip
)

add_executable(QTipExample
    examples/all_feature_test.cpp
)

target_include_directories(QTipExample
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(QTipExample
    PRIVATE
        QTip::QTip
)

add_executable(QTipShapeTest
    examples/shapetest.cpp
)

target_include_directories(QTipShapeTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(QTipShapeTest
    PRIVATE
        QTip::QTip
)

install(
    TARGETS QTip
    EXPORT QTipTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT QTipTargets
    FILE QTipTargets.cmake
    NAMESPACE QTip::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QTip
)
