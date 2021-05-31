########### recipientseditor test ###############

add_executable(recipienteditortest)
target_sources(recipienteditortest PRIVATE recipientseditortest.cpp)
target_link_libraries(recipienteditortest KF5::KIOCore KF5::AkonadiContact KF5::MessageComposer KF5::Libkdepim KF5::Completion KF5::Ldap KF5::I18n)


########### recipientseditor test ###############

add_executable(composerlineedittest)
target_sources(composerlineedittest PRIVATE testcomposerlineedit.cpp)
ecm_mark_as_test(composerlineedittest)
target_link_libraries(composerlineedittest KF5::MessageComposer  KF5::PimCommonAkonadi KF5::Completion KF5::Ldap KF5::I18n)


