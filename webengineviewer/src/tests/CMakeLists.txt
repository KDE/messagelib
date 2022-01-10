# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(webenginetest testwebengine.cpp)

target_link_libraries(webenginetest
    Qt${QT_MAJOR_VERSION}::Widgets KF5::WebEngineViewer Qt${QT_MAJOR_VERSION}::WebEngine Qt${QT_MAJOR_VERSION}::WebEngineWidgets
    )

#####
add_executable(webenginescripttest testwebenginescript.cpp)

target_link_libraries(webenginescripttest
    Qt${QT_MAJOR_VERSION}::Widgets KF5::WebEngineViewer Qt${QT_MAJOR_VERSION}::WebEngine Qt${QT_MAJOR_VERSION}::WebEngineWidgets
    )

####
add_executable(selectedtextwebenginetest testselectedtextwebengine.cpp)

target_link_libraries(selectedtextwebenginetest
    Qt${QT_MAJOR_VERSION}::Widgets KF5::WebEngineViewer Qt${QT_MAJOR_VERSION}::WebEngine Qt${QT_MAJOR_VERSION}::WebEngineWidgets
    )
####
add_executable(testdndwebenginetest testdndwebengine.cpp)

target_link_libraries(testdndwebenginetest
    Qt${QT_MAJOR_VERSION}::Widgets KF5::WebEngineViewer Qt${QT_MAJOR_VERSION}::WebEngine Qt${QT_MAJOR_VERSION}::WebEngineWidgets
    )

#####

add_executable(testselectionchangedwebengine testselectionchangedwebengine.cpp)

target_link_libraries(testselectionchangedwebengine
    Qt${QT_MAJOR_VERSION}::Widgets KF5::WebEngineViewer Qt${QT_MAJOR_VERSION}::WebEngine Qt${QT_MAJOR_VERSION}::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )

####

add_definitions(-DPICSRC="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(testwebengineviewinterceptor)
target_sources(testwebengineviewinterceptor PRIVATE
    testwebengineviewinterceptor.cpp
    testwebengineviewinterceptor.qrc
    )

target_link_libraries(testwebengineviewinterceptor
    Qt${QT_MAJOR_VERSION}::Widgets Qt${QT_MAJOR_VERSION}::WebEngineWidgets 
    )

