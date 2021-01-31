ecm_add_test(findbarbasetest.cpp
    TEST_NAME findbarbasetest
    NAME_PREFIX "webengineviewer-findbar-"
    LINK_LIBRARIES KF5WebEngineViewer Qt::Test Qt::Widgets Qt::Gui KF5::Completion KF5::I18n KF5::ConfigWidgets KF5::PimCommon
)
set_property(TARGET findbarbasetest PROPERTY INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/webengineviewer ${CMAKE_BINARY_DIR}/webengineviewer/src)
