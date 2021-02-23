/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmenu.h"
#include "remotecontentconfiguredialog.h"
#include <KLocalizedString>
#include <QAction>
#include <QPointer>

using namespace MessageViewer;
RemoteContentMenu::RemoteContentMenu(QWidget *parent)
    : QMenu(parent)
{
    initialize();
    setTitle(i18n("Remote Content"));
    initialize();
    updateMenu();
}

RemoteContentMenu::~RemoteContentMenu()
{
}

void RemoteContentMenu::initialize()
{
    mConfigureRemoteContentAction = new QAction(i18n("Configure"), this);
    connect(mConfigureRemoteContentAction, &QAction::triggered, this, &RemoteContentMenu::slotConfigure);
}

void RemoteContentMenu::slotConfigure()
{
    QPointer<MessageViewer::RemoteContentConfigureDialog> remoteContentDialog = new MessageViewer::RemoteContentConfigureDialog(this);
    remoteContentDialog->exec();
    delete remoteContentDialog;
}

QStringList RemoteContentMenu::urls() const
{
    return mUrls;
}

void RemoteContentMenu::setUrls(const QStringList &urls)
{
    mUrls = urls;
}

void RemoteContentMenu::updateMenu()
{
    clear();
    for (const QString &url : qAsConst(mUrls)) {
        QAction *act = addAction(i18n("Authorize %1", url));
        connect(act, &QAction::triggered, this, [this, url]() {
            authorize(url);
        });
    }
    addSeparator();
    addAction(mConfigureRemoteContentAction);
}

void RemoteContentMenu::authorize(const QString &url)
{
    // TODO
}

void RemoteContentMenu::clearUrls()
{
    mUrls.clear();
}

void RemoteContentMenu::appendUrl(const QString &url)
{
    if (!mUrls.contains(url)) {
        mUrls.append(url);
    }
}
