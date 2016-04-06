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

######
if (NOT MESSAGEVIEWER_USE_QTWEBENGINE)
set(webengineaccesskey_test_SRCS
    testwebengineaccesskey.cpp
    )

add_executable(webengineaccesskeytest ${webengineaccesskey_test_SRCS})

target_link_libraries(webengineaccesskeytest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::WebKit Qt5::WebKitWidgets
    )
endif()
####

#laurent move to messageviewer test
if (0)
set(scrolladdattachment_test_SRCS
    testwebenginescrolladdattachment.cpp
    )

add_executable(scrolladdattachmenttest ${scrolladdattachment_test_SRCS})

target_link_libraries(scrolladdattachmenttest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes
    )

####
set(testmailwebengine_test_SRCS
    testmailwebengine.cpp
    )

add_executable(testmailwebengine ${testmailwebengine_test_SRCS})

target_link_libraries(testmailwebengine
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes
    )
endif()
