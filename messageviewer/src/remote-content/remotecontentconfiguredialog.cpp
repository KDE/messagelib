/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

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
#if TEXTADDONSWIDGETS_VERSION >= QT_VERSION_CHECK(2, 1, 49)
    TextAddonsWidgets::LoadDialogSizeUtils::manageDialogSize(this, QLatin1StringView(myRemoteContentConfigureConfigGroupName), QSize(600, 200));
#else
    create(); // ensure a window is created
    TextAddonsWidgets::LoadDialogSizeUtils::loadDialogSizeScaled(this, QLatin1StringView(myRemoteContentConfigureConfigGroupName), 600, 200);
#endif
}

void RemoteContentConfigureDialog::writeConfig()
{
#if TEXTADDONSWIDGETS_VERSION < QT_VERSION_CHECK(2, 1, 49)
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myRemoteContentConfigureConfigGroupName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
#endif
}

#include "moc_remotecontentconfiguredialog.cpp"
