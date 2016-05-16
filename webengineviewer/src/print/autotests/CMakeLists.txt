set(print_autotest_SRCS)
ecm_qt_declare_logging_category(print_autotest_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME log_webengineviewer)

ecm_add_test(printpreviewdialogtest.cpp  ../printpreviewdialog.cpp ../printpreviewpageviewer.cpp ../printpreviewpagewidget.cpp ${print_autotest_SRCS}
    TEST_NAME printpreviewdialogtest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n KF5::WidgetsAddons Poppler::Qt5 KF5::ConfigCore
    )

ecm_add_test(printpreviewpageviewertest.cpp  ../printpreviewpageviewer.cpp ${print_autotest_SRCS}
    TEST_NAME printpreviewpagetest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n KF5::WidgetsAddons Poppler::Qt5 KF5::ConfigCore
    )

ecm_add_test(printpreviewpagewidgettest.cpp  ../printpreviewpagewidget.cpp ../printpreviewpageviewer.cpp ${print_autotest_SRCS}
    TEST_NAME printpreviewpagewidgettest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n KF5::WidgetsAddons Poppler::Qt5 KF5::ConfigCore
    )

ecm_add_test(printwebengineviewjobtest.cpp  ../printwebengineviewjob.cpp ${print_autotest_SRCS}
    TEST_NAME printwebengineviewjobtest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::WebEngineWidgets Qt5::PrintSupport
    )

ecm_add_test(printconfiguredialogtest.cpp  ../printconfiguredialog.cpp ../printconfigurewidget.cpp ${print_autotest_SRCS}
    TEST_NAME printconfiguredialogtest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n Qt5::PrintSupport
    )

ecm_add_test(printconfigurewidgettest.cpp  ../printconfigurewidget.cpp ${print_autotest_SRCS}
    TEST_NAME printconfigurewidgettest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n Qt5::PrintSupport
    )
