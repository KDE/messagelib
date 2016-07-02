add_definitions( -DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

set(scrolladdattachment_test_SRCS
    testwebenginescrolladdattachment.cpp
    )

add_executable(scrolladdattachmenttest ${scrolladdattachment_test_SRCS})

target_link_libraries(scrolladdattachmenttest
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )

####
set(testmailwebengine_test_SRCS
    testmailwebengine.cpp
    )

add_executable(testmailwebengine ${testmailwebengine_test_SRCS})

target_link_libraries(testmailwebengine
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )

#####
#set(testwebengineaccesskey_test_SRCS
#    testwebengineaccesskey.cpp
#    )

#add_executable(testwebengineaccesskey ${testwebengineaccesskey_test_SRCS})

#target_link_libraries(testwebengineaccesskey
#    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::MessageViewer
#    )

####
set(testmailwebengineselection_test_SRCS
    testmailwebengineselection.cpp
    )

add_executable(testmailwebengineselection ${testmailwebengineselection_test_SRCS})

target_link_libraries(testmailwebengineselection
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )
######

set(testmaildndattachment_test_SRCS
    testmaildndattachment.cpp
    )
add_executable(testmaildndattachment ${testmaildndattachment_test_SRCS})

target_link_libraries(testmaildndattachment
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )

#####
set(testjquerysupportmailwebengine_test_SRCS
    testjquerysupportmailwebengine.cpp
    )
add_executable(testjquerysupportmailwebengine ${testjquerysupportmailwebengine_test_SRCS})

target_link_libraries(testjquerysupportmailwebengine
    Qt5::Widgets KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )



