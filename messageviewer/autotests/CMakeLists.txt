# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DMAIL_DATA_DIR="${CMAKE_SOURCE_DIR}/mimetreeparser/autotests/data")
add_definitions(-DGRANTLEETHEME_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

set(test_libmessageviewer_SRCS)
ecm_qt_declare_logging_category(test_libmessageviewer_SRCS HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME org.kde.pim.messageviewer)

# convenience macro to add qtest unit tests
macro(add_messageviewer_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} setupenv.cpp util.cpp ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KPim6::MessageViewer KPim6::Libkleo QGpgmeQt6 Qt::Test KF6::KIOCore KPim6::Mime KPim6::AkonadiCore KF6::CalendarCore KPim6::WebEngineViewer
)
endmacro()

macro(add_messageviewer_class_unittest _source _additionalSource)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_additionalSource} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KPim6::MessageViewer Qt::Test KF6::IconThemes KF6::XmlGui KF6::I18n
)
endmacro()

add_messageviewer_unittest( messagedisplayformatattributetest.cpp)

# convenience macro to add qtest unit tests
macro(add_messageviewer_mailsourceviewbrowserwidget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ../src/widgets/mailsourceviewtextbrowserwidget.cpp ../src/findbar/findbarsourceview.cpp ${test_libmessageviewer_SRCS} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES Qt::Test Qt::Gui Qt::Widgets KF6::KIOCore KPim6::Mime KPim6::AkonadiCore KF6::CalendarCore KPim6::PimTextEdit
        KPim6::PimCommon KPim6::MessageViewer KPim6::WebEngineViewer KF6::SyntaxHighlighting KF6::TextAddonsWidgets
        KF6::I18n KF6::IconThemes
)
    if(TARGET KF6::TextEditTextToSpeech)
        target_link_libraries(${_name} KF6::TextEditTextToSpeech)
    endif()
endmacro()


add_messageviewer_mailsourceviewbrowserwidget_unittest( mailsourceviewtextbrowserwidgettest.cpp)
########### viewertest_gui ###############

add_akonadi_isolated_test_advanced(viewertest.cpp "" "KPim6::MessageViewer;KF6::XmlGui")
add_akonadi_isolated_test_advanced(viewergrantleethemesupporttest.cpp "" "KPim6::MessageViewer;KF6::XmlGui")
add_akonadi_isolated_test_advanced(urlhandlermanagertest.cpp "util.cpp" "KPim6::MessageViewer;KPim6::Libkleo;QGpgmeQt6;KF6::KIOCore;KPim6::Mime;KPim6::AkonadiCore;KF6::CalendarCore;KPim6::WebEngineViewer")
