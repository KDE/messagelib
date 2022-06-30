# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DMAIL_DATA_DIR="${CMAKE_SOURCE_DIR}/mimetreeparser/autotests/data" )
add_definitions( -DGRANTLEETHEME_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

set(test_libmessageviewer_SRCS)
ecm_qt_declare_logging_category(test_libmessageviewer_SRCS HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME org.kde.pim.messageviewer)

if (QT_MAJOR_VERSION STREQUAL "6")
    set(qgpgme_lib QGpgmeQt6)
else()
    set(qgpgme_lib QGpgme)
endif()

# convenience macro to add qtest unit tests
macro(add_messageviewer_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} setupenv.cpp util.cpp ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::Libkleo ${qgpgme_lib} Qt${QT_MAJOR_VERSION}::Test KF5::KIOCore KF5::Mime KF5::AkonadiCore KF5::CalendarCore KF5::WebEngineViewer
    )
endmacro ()

macro(add_messageviewer_class_unittest _source _additionalSource)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_additionalSource} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer Qt${QT_MAJOR_VERSION}::Test KF5::IconThemes KF5::XmlGui KF5::I18n
    )
endmacro ()

add_messageviewer_unittest( messagedisplayformatattributetest.cpp )

# convenience macro to add qtest unit tests
macro(add_messageviewer_mailsourceviewbrowserwidget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ../src/widgets/mailsourceviewtextbrowserwidget.cpp ../src/findbar/findbarsourceview.cpp ${test_libmessageviewer_SRCS} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Gui Qt${QT_MAJOR_VERSION}::Widgets KF5::KIOCore KF5::Mime KF5::AkonadiCore KF5::CalendarCore KF5::PimTextEdit KF5::PimCommon KF5::MessageViewer KF5::WebEngineViewer KF5::SyntaxHighlighting KF5::I18n KF5::IconThemes
    )
endmacro ()


add_messageviewer_mailsourceviewbrowserwidget_unittest( mailsourceviewtextbrowserwidgettest.cpp )
########### viewertest_gui ###############

add_akonadi_isolated_test_advanced(viewertest.cpp "" "KF5::MessageViewer;KF5::XmlGui")
add_akonadi_isolated_test_advanced(viewergrantleethemesupporttest.cpp "" "KF5::MessageViewer;KF5::XmlGui")
add_akonadi_isolated_test_advanced(urlhandlermanagertest.cpp "util.cpp" "KF5::MessageViewer;KF5::Libkleo;${qgpgme_lib};KF5::KIOCore;KF5::Mime;KF5::AkonadiCore;KF5::CalendarCore;KF5::WebEngineViewer")
