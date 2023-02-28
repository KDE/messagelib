# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
########### recipientseditor test ###############

add_executable(recipienteditortest)
target_sources(recipienteditortest PRIVATE recipientseditortest.cpp recipientseditortest.h)
target_link_libraries(recipienteditortest KF${KF_MAJOR_VERSION}::KIOCore KPim${KF_MAJOR_VERSION}::AkonadiContact KF5::MessageComposer KPim${KF_MAJOR_VERSION}::Libkdepim KF${KF_MAJOR_VERSION}::Completion KF${KF_MAJOR_VERSION}::I18n)


########### recipientseditor test ###############

add_executable(composerlineedittest)
target_sources(composerlineedittest PRIVATE testcomposerlineedit.cpp)
ecm_mark_as_test(composerlineedittest)
target_link_libraries(composerlineedittest KF5::MessageComposer  KF5::PimCommonAkonadi KF${KF_MAJOR_VERSION}::Completion KF${KF_MAJOR_VERSION}::I18n)


