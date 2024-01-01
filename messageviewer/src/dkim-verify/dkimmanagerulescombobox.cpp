/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerulescombobox.h"
#include <KLocalizedString>
using namespace MessageViewer;
DKIMManageRulesComboBox::DKIMManageRulesComboBox(QWidget *parent)
    : QComboBox(parent)
{
    init();
}

DKIMManageRulesComboBox::~DKIMManageRulesComboBox() = default;

void DKIMManageRulesComboBox::init()
{
    addItem(i18n("Must be signed"), QVariant::fromValue(MessageViewer::DKIMRule::RuleType::MustBeSigned));
    addItem(i18n("Can be signed"), QVariant::fromValue(MessageViewer::DKIMRule::RuleType::CanBeSigned));
    addItem(i18n("Ignore if not signed"), QVariant::fromValue(MessageViewer::DKIMRule::RuleType::IgnoreEmailNotSigned));
}

MessageViewer::DKIMRule::RuleType DKIMManageRulesComboBox::ruleType() const
{
    return currentData().value<MessageViewer::DKIMRule::RuleType>();
}

void DKIMManageRulesComboBox::setRuleType(MessageViewer::DKIMRule::RuleType type)
{
    const int index = findData(QVariant::fromValue(type));
    if (index != -1) {
        setCurrentIndex(index);
    }
}

#include "moc_dkimmanagerulescombobox.cpp"
