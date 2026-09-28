/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfiguredialog.h"
#include "remotecontentconfigurewidget.h"
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <TextAddonsWidgets/LoadDialogSizeUtils>

using namespace Qt::Literals::StringLiterals;
namespace
{
static const char myRemoteContentConfigureConfigGroupName[] = "RemoteContentConfigureDialog";
}

using namespace MessageViewer;
RemoteContentConfigureDialog::RemoteContentConfigureDialog(QWidget *parent)
    : QDialog(parent)
    , mRemoteContentConfigureWidget(new RemoteContentConfigureWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Configure Remote Content"));
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName("mainLayout"_L1);

    mRemoteContentConfigureWidget->setObjectName("mRemoteContentConfigureWidget"_L1);
    mainLayout->addWidget(mRemoteContentConfigureWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName("buttonBox"_L1);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RemoteContentConfigureDialog::slotAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RemoteContentConfigureDialog::reject);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

RemoteContentConfigureDialog::~RemoteContentConfigureDialog() = default;

void RemoteContentConfigureDialog::slotAccept()
{
    mRemoteContentConfigureWidget->saveSettings();
    accept();
}

void RemoteContentConfigureDialog::readConfig()
{
    TextAddonsWidgets::LoadDialogSizeUtils::manageDialogSize(this, QLatin1StringView(myRemoteContentConfigureConfigGroupName), QSize(600, 200));
}

#include "moc_remotecontentconfiguredialog.cpp"
