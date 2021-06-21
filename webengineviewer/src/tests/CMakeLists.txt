add_executable(webenginetest testwebengine.cpp)

target_link_libraries(webenginetest
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets
    )

#####
add_executable(webenginescripttest testwebenginescript.cpp)

target_link_libraries(webenginescripttest
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets
    )

####
add_executable(selectedtextwebenginetest testselectedtextwebengine.cpp)

target_link_libraries(selectedtextwebenginetest
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets
    )
####
add_executable(testdndwebenginetest testdndwebengine.cpp)

target_link_libraries(testdndwebenginetest
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets
    )

#####

add_executable(testselectionchangedwebengine testselectionchangedwebengine.cpp)

target_link_libraries(testselectionchangedwebengine
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )

####

add_definitions(-DPICSRC="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(testwebengineviewinterceptor)
target_sources(testwebengineviewinterceptor PRIVATE
    testwebengineviewinterceptor.cpp
    testwebengineviewinterceptor.qrc
    )

target_link_libraries(testwebengineviewinterceptor
    Qt::Widgets Qt::WebEngineWidgets 
    )

