/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimruledialog.h"
#include "dkimrulewidget.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
static const char myDKIMRuleDialogConfigGroupName[] = "DKIMRuleDialog";
}
using namespace MessageViewer;
DKIMRuleDialog::DKIMRuleDialog(QWidget *parent)
    : QDialog(parent)
    , mRuleWidget(new DKIMRuleWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Add Rule"));

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainlayout"));

    mRuleWidget->setObjectName(QLatin1StringView("rulewidget"));
    mainLayout->addWidget(mRuleWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QLatin1StringView("buttonBox"));
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

DKIMRuleDialog::~DKIMRuleDialog()
{
    writeConfig();
}

void DKIMRuleDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myDKIMRuleDialogConfigGroupName));
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void DKIMRuleDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myDKIMRuleDialogConfigGroupName));
    group.writeEntry("Size", size());
    group.sync();
}

void DKIMRuleDialog::loadRule(const MessageViewer::DKIMRule &rule)
{
    setWindowTitle(i18nc("@title:window", "Modify Rule"));
    mRuleWidget->loadRule(rule);
}

MessageViewer::DKIMRule DKIMRuleDialog::rule() const
{
    return mRuleWidget->rule();
}

#include "moc_dkimruledialog.cpp"
