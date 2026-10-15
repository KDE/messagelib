/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimruledialog.h"
#include "dkimrulewidget.h"

#include <KLocalizedString>
#include <MessageCore/DKIMRule>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <TextAddonsWidgets/LoadDialogSizeUtils>

using namespace Qt::Literals::StringLiterals;

namespace
{
const char myDKIMRuleDialogConfigGroupName[] = "DKIMRuleDialog";
}
using namespace MessageViewer;
DKIMRuleDialog::DKIMRuleDialog(QWidget *parent)
    : QDialog(parent)
    , mRuleWidget(new DKIMRuleWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Add Rule"));

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName("mainlayout"_L1);

    mRuleWidget->setObjectName("rulewidget"_L1);
    mainLayout->addWidget(mRuleWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName("buttonBox"_L1);
    mainLayout->addWidget(buttonBox);

    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setEnabled(false);

    connect(mRuleWidget, &DKIMRuleWidget::updateOkButtonRequested, this, [this](bool enabled) {
        mOkButton->setEnabled(enabled);
    });

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DKIMRuleDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DKIMRuleDialog::reject);
    readConfig();
}

DKIMRuleDialog::~DKIMRuleDialog() = default;

void DKIMRuleDialog::readConfig()
{
    TextAddonsWidgets::LoadDialogSizeUtils::manageDialogSize(this, QLatin1StringView(myDKIMRuleDialogConfigGroupName), QSize(600, 400));
}

void DKIMRuleDialog::loadRule(const MessageCore::DKIMRule &rule)
{
    setWindowTitle(i18nc("@title:window", "Modify Rule"));
    mRuleWidget->loadRule(rule);
}

MessageCore::DKIMRule DKIMRuleDialog::rule() const
{
    return mRuleWidget->rule();
}

#include "moc_dkimruledialog.cpp"
