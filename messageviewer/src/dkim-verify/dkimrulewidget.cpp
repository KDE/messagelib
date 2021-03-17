/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimrulewidget.h"
#include "dkimmanagerulescombobox.h"
#include <KLocalizedString>
#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>

using namespace MessageViewer;
DKIMRuleWidget::DKIMRuleWidget(QWidget *parent)
    : QWidget(parent)
    , mEnabled(new QCheckBox(i18n("Enabled"), this))
    , mDomain(new QLineEdit(this))
    , mSignatureDomainIdentifier(new QLineEdit(this))
    , mFrom(new QLineEdit(this))
    , mListId(new QLineEdit(this))
    , mPriority(new QSpinBox(this))
    , mRuleType(new DKIMManageRulesComboBox(this))
{
    auto layout = new QFormLayout(this);
    layout->setObjectName(QStringLiteral("layout"));
    layout->setContentsMargins({});

    mEnabled->setObjectName(QStringLiteral("enabled"));
    mEnabled->setChecked(true);
    layout->addWidget(mEnabled);

    mDomain->setObjectName(QStringLiteral("domain"));
    mDomain->setClearButtonEnabled(true);
    layout->addRow(i18n("Domain:"), mDomain);
    connect(mDomain, &QLineEdit::textChanged, this, &DKIMRuleWidget::updateOkButton);

    mListId->setObjectName(QStringLiteral("listid"));
    mListId->setClearButtonEnabled(true);
    layout->addRow(i18n("List-Id:"), mListId);

    mFrom->setObjectName(QStringLiteral("from"));
    mFrom->setClearButtonEnabled(true);
    layout->addRow(i18n("From:"), mFrom);
    mFrom->setPlaceholderText(i18n("Use '*' to specify all emails from domain"));
    connect(mFrom, &QLineEdit::textChanged, this, &DKIMRuleWidget::updateOkButton);

    mSignatureDomainIdentifier->setObjectName(QStringLiteral("signaturedomainidentifier"));
    mSignatureDomainIdentifier->setClearButtonEnabled(true);
    layout->addRow(i18n("SDID:"), mSignatureDomainIdentifier);

    mRuleType->setObjectName(QStringLiteral("ruletype"));
    layout->addRow(i18n("Rule:"), mRuleType);

    mPriority->setObjectName(QStringLiteral("priority"));
    mPriority->setMinimum(1);
    mPriority->setMaximum(9999);
    mPriority->setValue(1000); // Default Value
    layout->addRow(i18n("Priority:"), mPriority);
}

DKIMRuleWidget::~DKIMRuleWidget()
{
}

void DKIMRuleWidget::updateOkButton()
{
    Q_EMIT updateOkButtonRequested(!mFrom->text().trimmed().isEmpty() && !mDomain->text().trimmed().isEmpty());
}

void DKIMRuleWidget::loadRule(const MessageViewer::DKIMRule &rule)
{
    mEnabled->setChecked(rule.enabled());
    mDomain->setText(rule.domain());
    mSignatureDomainIdentifier->setText(rule.signedDomainIdentifier().join(QLatin1Char(' ')));
    mFrom->setText(rule.from());
    mListId->setText(rule.listId());
    mRuleType->setRuleType(rule.ruleType());
    mPriority->setValue(rule.priority());
}

MessageViewer::DKIMRule DKIMRuleWidget::rule() const
{
    MessageViewer::DKIMRule rule;
    rule.setEnabled(mEnabled->isChecked());
    rule.setDomain(mDomain->text());
    rule.setSignedDomainIdentifier(mSignatureDomainIdentifier->text().split(QLatin1Char(' ')));
    rule.setFrom(mFrom->text());
    rule.setListId(mListId->text());
    rule.setRuleType(mRuleType->ruleType());
    rule.setPriority(mPriority->value());
    return rule;
}
