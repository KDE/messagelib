
set( kmail_statusbarlabeltoggledstate_source statusbarlabeltoggledstatetest.cpp)
add_executable(statusbarlabeltoggledstatetest ${kmail_statusbarlabeltoggledstate_source})
ecm_mark_as_test(kmail-statusbarlabeltoggledstatetest)
target_link_libraries( statusbarlabeltoggledstatetest Qt5::Test KF5::KIOWidgets KF5::I18n KF5::MessageComposer)

