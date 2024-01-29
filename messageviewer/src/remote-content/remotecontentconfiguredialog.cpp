/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfiguredialog.h"
#include "remotecontentconfigurewidget.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QWindow>

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
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));

    mRemoteContentConfigureWidget->setObjectName(QLatin1StringView("mRemoteContentConfigureWidget"));
    mainLayout->addWidget(mRemoteContentConfigureWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName(QLatin1StringView("buttonBox"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RemoteContentConfigureDialog::slotAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RemoteContentConfigureDialog::reject);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

RemoteContentConfigureDialog::~RemoteContentConfigureDialog()
{
    writeConfig();
}

void RemoteContentConfigureDialog::slotAccept()
{
    mRemoteContentConfigureWidget->saveSettings();
    accept();
}

void RemoteContentConfigureDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(600, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myRemoteContentConfigureConfigGroupName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void RemoteContentConfigureDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myRemoteContentConfigureConfigGroupName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
}

#include "moc_remotecontentconfiguredialog.cpp"
