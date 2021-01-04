/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfiguredialog.h"
#include "remotecontentconfigurewidget.h"
#include <KLocalizedString>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <KSharedConfig>

namespace {
static const char myRemoteContentConfigureConfigGroupName[] = "RemoteContentConfigureDialog";
}

using namespace MessageViewer;
RemoteContentConfigureDialog::RemoteContentConfigureDialog(QWidget *parent)
    : QDialog(parent)
    , mRemoteContentConfigureWidget(new RemoteContentConfigureWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Details"));
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    mRemoteContentConfigureWidget->setObjectName(QStringLiteral("mRemoteContentConfigureWidget"));
    mainLayout->addWidget(mRemoteContentConfigureWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RemoteContentConfigureDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RemoteContentConfigureDialog::reject);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

RemoteContentConfigureDialog::~RemoteContentConfigureDialog()
{
    writeConfig();
}

void RemoteContentConfigureDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "myRemoteContentConfigureConfigGroupName");
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void RemoteContentConfigureDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "myRemoteContentConfigureConfigGroupName");
    group.writeEntry("Size", size());
    group.sync();
}
