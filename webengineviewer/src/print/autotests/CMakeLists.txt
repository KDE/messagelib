ecm_add_test(printpreviewdialogtest.cpp  ../printpreviewdialog.cpp ../printpreviewpageviewer.cpp ../printpreviewpagewidget.cpp
    TEST_NAME printpreviewdialogtest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n KF5::WidgetsAddons Poppler::Qt5 KF5::ConfigCore
)

ecm_add_test(printpreviewpageviewertest.cpp  ../printpreviewpageviewer.cpp
    TEST_NAME printpreviewpagetest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n KF5::WidgetsAddons Poppler::Qt5 KF5::ConfigCore
)

ecm_add_test(printpreviewpagewidgettest.cpp  ../printpreviewpagewidget.cpp ../printpreviewpageviewer.cpp
    TEST_NAME printpreviewpagewidgettest
    NAME_PREFIX "webengineviewer-print-"
    LINK_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Gui KF5::I18n KF5::WidgetsAddons Poppler::Qt5 KF5::ConfigCore
)
