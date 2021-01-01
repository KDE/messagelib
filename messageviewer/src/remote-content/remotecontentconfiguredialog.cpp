/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfiguredialog.h"
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
{
    setWindowTitle(i18nc("@title:window", "Details"));
    setAttribute(Qt::WA_DeleteOnClose);
    auto mainLayout = new QVBoxLayout(this);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
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
