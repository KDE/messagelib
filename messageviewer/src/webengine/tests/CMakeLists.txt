set(webengine_test_SRCS
    main.cpp
    testwebengine.cpp
    )

add_executable(webenginetest ${webengine_test_SRCS}) 

target_link_libraries(webenginetest
    Qt5::Widgets KF5::MessageViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )
