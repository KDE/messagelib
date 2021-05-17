add_definitions( -DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

set(scrolladdattachment_test_SRCS
    testwebenginescrolladdattachment.cpp
    )

add_executable(scrolladdattachmenttest ${scrolladdattachment_test_SRCS})

target_link_libraries(scrolladdattachmenttest
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )

####
set(testmailwebengine_test_SRCS
    testmailwebengine.cpp
    )

add_executable(testmailwebengine ${testmailwebengine_test_SRCS})

target_link_libraries(testmailwebengine
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer Qt::PrintSupport KF5::I18n
)

#####
#set(testwebengineaccesskey_test_SRCS
#    testwebengineaccesskey.cpp
#    )

#add_executable(testwebengineaccesskey ${testwebengineaccesskey_test_SRCS})

#target_link_libraries(testwebengineaccesskey
#    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets KF5::MessageViewer
#    )

####
set(testmailwebengineselection_test_SRCS
    testmailwebengineselection.cpp
    )

add_executable(testmailwebengineselection ${testmailwebengineselection_test_SRCS})

target_link_libraries(testmailwebengineselection
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )
######

set(testmaildndattachment_test_SRCS
    testmaildndattachment.cpp
    )
add_executable(testmaildndattachment ${testmaildndattachment_test_SRCS})

target_link_libraries(testmaildndattachment
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )

#####

set(testmailmboxwebengine_test_SRCS
    testmailmboxwebengine.cpp
    )
add_executable(testmailmboxwebengine ${testmailmboxwebengine_test_SRCS})

target_link_libraries(testmailmboxwebengine
    Qt::Widgets KF5::WebEngineViewer Qt::WebEngine Qt::WebEngineWidgets KF5::XmlGui KF5::IconThemes KF5::MessageViewer
    )
