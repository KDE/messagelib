# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
########### recipientseditor test ###############

add_executable(recipienteditortest)
target_sources(recipienteditortest PRIVATE recipientseditortest.cpp recipientseditortest.h)
target_link_libraries(recipienteditortest KF6::KIOCore KPim6::AkonadiContactWidgets KPim6::MessageComposer KPim6::Libkdepim KF6::Completion KF6::I18n)


########### recipientseditor test ###############

add_executable(composerlineedittest)
target_sources(composerlineedittest PRIVATE testcomposerlineedit.cpp)
ecm_mark_as_test(composerlineedittest)
target_link_libraries(composerlineedittest KPim6::MessageComposer  KPim6::PimCommonAkonadi KF6::Completion KF6::I18n)


