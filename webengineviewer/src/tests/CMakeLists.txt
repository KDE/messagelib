# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(webenginetest testwebengine.cpp testwebengine.h)

target_link_libraries(webenginetest
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets
)

#####
add_executable(webenginescripttest testwebenginescript.cpp testwebenginescript.h)

target_link_libraries(webenginescripttest
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets
)

####
add_executable(selectedtextwebenginetest testselectedtextwebengine.cpp testselectedtextwebengine.h)

target_link_libraries(selectedtextwebenginetest
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets
)
####
add_executable(testdndwebenginetest testdndwebengine.cpp testdndwebengine.h)

target_link_libraries(testdndwebenginetest
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets
)

#####

add_executable(testselectionchangedwebengine testselectionchangedwebengine.cpp testselectionchangedwebengine.h)

target_link_libraries(testselectionchangedwebengine
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets KF6::XmlGui KF6::IconThemes KPim6::MessageViewer
)

####

add_definitions(-DPICSRC="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(testwebengineviewinterceptor)
target_sources(testwebengineviewinterceptor PRIVATE
    testwebengineviewinterceptor.cpp
    testwebengineviewinterceptor.h
    testwebengineviewinterceptor.qrc
)

target_link_libraries(testwebengineviewinterceptor
    Qt::Widgets Qt::WebEngineWidgets 
)

