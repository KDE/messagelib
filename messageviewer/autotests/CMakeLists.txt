set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

add_definitions(-DKMAIL_UNITTESTS=YES)

# convenience macro to add qtest unit tests
macro(add_messageviewer_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} util.cpp setupenv.cpp testcsshelper.cpp
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::Libkleo KF5::QGpgme Qt5::Test Qt5::WebKitWidgets KF5::KIOCore KF5::Mime KF5::AkonadiCore KF5::AkonadiNotes KF5::CalendarCore
    )
endmacro ()

macro(add_messageviewer_class_unittest _source _additionalSource)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_additionalSource}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer Qt5::Test KF5::IconThemes KF5::XmlGui Qt5::WebKitWidgets KF5::I18n
    )
endmacro ()

add_messageviewer_unittest( messagedisplayformatattributetest.cpp )
add_messageviewer_class_unittest(zoomactionmenutest.cpp "../src/widgets/zoomactionmenu.cpp")

# convenience macro to add qtest unit tests
macro(add_messageviewer_mailsourceviewbrowserwidget_unittest _source)
    set(_test ${_source} util.cpp)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} util.cpp ../src/widgets/mailsourceviewtextbrowserwidget.cpp ../src/findbar/findbarsourceview.cpp
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES Qt5::Test Qt5::Gui Qt5::Widgets KF5::KIOCore KF5::Mime KF5::AkonadiCore KF5::CalendarCore KF5::PimTextEdit KF5::PimCommon KF5::MessageViewer KF5::WebEngineViewer
    )
endmacro ()


add_messageviewer_mailsourceviewbrowserwidget_unittest( mailsourceviewtextbrowserwidgettest.cpp )
########### viewertest_gui ###############

set(KDEPIMLIBS_RUN_ISOLATED_TESTS TRUE)
set(KDEPIMLIBS_RUN_SQLITE_ISOLATED_TESTS TRUE)

add_akonadi_isolated_test_advanced(viewertest.cpp "" "KF5::MessageViewer;KF5::XmlGui")

