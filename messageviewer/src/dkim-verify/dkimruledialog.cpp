/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

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

namespace {
static const char myConfigGroupName[] = "DKIMRuleDialog";
}
using namespace MessageViewer;
DKIMRuleDialog::DKIMRuleDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Add Rule"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));

    mRuleWidget = new DKIMRuleWidget(this);
    mRuleWidget->setObjectName(QStringLiteral("rulewidget"));
    mainLayout->addWidget(mRuleWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    mainLayout->addWidget(buttonBox);

    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setEnabled(false);

    connect(mRuleWidget, &DKIMRuleWidget::updateOkButtonRequested, this, [this](bool enabled) {
        mOkButton->setEnabled(enabled);
    });

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DKIMRuleDialog::slotAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DKIMRuleDialog::reject);
    readConfig();
}

DKIMRuleDialog::~DKIMRuleDialog()
{
    writeConfig();
}

void DKIMRuleDialog::slotAccepted()
{
    accept();
}

void DKIMRuleDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void DKIMRuleDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);
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
