/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

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
    addItem(i18n("Must be signed"), QVariant::fromValue(MessageCore::DKIMRule::RuleType::MustBeSigned));
    addItem(i18n("Can be signed"), QVariant::fromValue(MessageCore::DKIMRule::RuleType::CanBeSigned));
    addItem(i18n("Ignore if not signed"), QVariant::fromValue(MessageCore::DKIMRule::RuleType::IgnoreEmailNotSigned));
}

MessageCore::DKIMRule::RuleType DKIMManageRulesComboBox::ruleType() const
{
    return currentData().value<MessageCore::DKIMRule::RuleType>();
}

void DKIMManageRulesComboBox::setRuleType(MessageCore::DKIMRule::RuleType type)
{
    const int index = findData(QVariant::fromValue(type));
    if (index != -1) {
        setCurrentIndex(index);
    }
}

#include "moc_dkimmanagerulescombobox.cpp"
