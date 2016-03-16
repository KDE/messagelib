set(webengine_test_SRCS
    testwebengine.cpp
    )

add_executable(webenginetest ${webengine_test_SRCS}) 

target_link_libraries(webenginetest
    Qt5::Widgets KF5::MessageViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )


set(webenginescript_test_SRCS
    testwebenginescript.cpp
    )

add_executable(webenginescripttest ${webenginescript_test_SRCS})

target_link_libraries(webenginescripttest
    Qt5::Widgets KF5::MessageViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )
