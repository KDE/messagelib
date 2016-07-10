set(webengine_test_SRCS
    testwebengine.cpp
    )

add_executable(webenginetest ${webengine_test_SRCS}) 

target_link_libraries(webenginetest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )

#####
set(webenginescript_test_SRCS
    testwebenginescript.cpp
    )

add_executable(webenginescripttest ${webenginescript_test_SRCS})

target_link_libraries(webenginescripttest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )

####
set(selectedtextwebengine_test_SRCS
    testselectedtextwebengine.cpp
    )

add_executable(selectedtextwebenginetest ${selectedtextwebengine_test_SRCS})

target_link_libraries(selectedtextwebenginetest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )


###
set(jquerysupportwebengine_test_SRCS
    testjquerysupportwebengine.cpp
    )

add_executable(jquerysupportwebenginetest ${jquerysupportwebengine_test_SRCS})

target_link_libraries(jquerysupportwebenginetest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )

##
set(testdndwebengine_test_SRCS
    testdndwebengine.cpp
    )

add_executable(testdndwebenginetest ${testdndwebengine_test_SRCS})

target_link_libraries(testdndwebenginetest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets
    )
