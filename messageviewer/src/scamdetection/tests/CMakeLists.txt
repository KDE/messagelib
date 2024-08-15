# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include(ECMAddTests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(scamdetectionwebengine_gui)
target_sources(scamdetectionwebengine_gui PRIVATE scamdetectionwebengine_gui.cpp scamdetectionwebengine_gui.h)
target_link_libraries(scamdetectionwebengine_gui
    KF6::KIOCore
    KPim6::MessageViewer
    Qt::WebEngineWidgets
    KF6::I18n
)

