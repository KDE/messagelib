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
