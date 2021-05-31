
add_executable(statusbarlabeltoggledstatetest statusbarlabeltoggledstatetest.cpp)
ecm_mark_as_test(kmail-statusbarlabeltoggledstatetest)
target_link_libraries( statusbarlabeltoggledstatetest Qt::Test KF5::KIOWidgets KF5::I18n KF5::MessageComposer)

