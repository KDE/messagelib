# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_definitions(-DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

set(scrolladdattachment_test_SRCS
    testwebenginescrolladdattachment.cpp
    testwebenginescrolladdattachment.h
)

add_executable(scrolladdattachmenttest ${scrolladdattachment_test_SRCS})

target_link_libraries(scrolladdattachmenttest
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets KF6::XmlGui KF6::IconThemes KPim6::MessageViewer
)

####
set(testmailwebengine_test_SRCS
    testmailwebengine.cpp
    testmailwebengine.h
)

add_executable(testmailwebengine ${testmailwebengine_test_SRCS})

target_link_libraries(testmailwebengine
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets KF6::XmlGui KF6::IconThemes KPim6::MessageViewer Qt::PrintSupport KF6::I18n
)

#####
#set(testwebengineaccesskey_test_SRCS
#    testwebengineaccesskey.cpp
#   )

#add_executable(testwebengineaccesskey ${testwebengineaccesskey_test_SRCS})

#target_link_libraries(testwebengineaccesskey
#    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets KPim6::MessageViewer
#   )

####
set(testmailwebengineselection_test_SRCS
    testmailwebengineselection.cpp
    testmailwebengineselection.h
)

add_executable(testmailwebengineselection ${testmailwebengineselection_test_SRCS})

target_link_libraries(testmailwebengineselection
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets KF6::XmlGui KF6::IconThemes KPim6::MessageViewer
)
######

set(testmaildndattachment_test_SRCS
    testmaildndattachment.cpp
    testmaildndattachment.h
)
add_executable(testmaildndattachment ${testmaildndattachment_test_SRCS})

target_link_libraries(testmaildndattachment
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets KF6::XmlGui KF6::IconThemes KPim6::MessageViewer
)

#####

set(testmailmboxwebengine_test_SRCS
    testmailmboxwebengine.cpp
    testmailmboxwebengine.h
)
add_executable(testmailmboxwebengine ${testmailmboxwebengine_test_SRCS})

target_link_libraries(testmailmboxwebengine
    Qt::Widgets KPim6::WebEngineViewer  Qt::WebEngineWidgets KF6::XmlGui KF6::IconThemes KPim6::MessageViewer
)
