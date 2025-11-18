cmake_minimum_required(VERSION 3.16)
project(NodeDataFlowEditorDemo LANGUAGES CXX)

set(TARGET_NAME NodeDataFlowEditorDemo)

# Find your library
add_subdirectory(../NodeDataFlowEditor NodeDataFlowEditor_build)

# Find Qt (reuse your library Qt setup)
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Add demo executable
add_executable(${TARGET_NAME} main.cpp MainWindow.cpp MainWindow.hpp PortTags.hpp)

# Include directories
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/..
)

# Link against your library and Qt
target_link_libraries(${TARGET_NAME}
    PRIVATE
        NodeDataFlowEditor
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
