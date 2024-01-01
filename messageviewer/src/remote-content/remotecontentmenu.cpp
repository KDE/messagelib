/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmenu.h"
#include "remote-content/remotecontentmanager.h"
#include "remotecontentconfiguredialog.h"
#include "remotecontentinfo.h"
#include <KLocalizedString>
#include <QAction>

using namespace MessageViewer;
RemoteContentMenu::RemoteContentMenu(QWidget *parent)
    : QMenu(parent)
    , mConfigureRemoteContentAction(new QAction(QIcon::fromTheme(QStringLiteral("settings-configure")), i18n("Configure"), this))
{
    setTitle(i18n("Remote Content"));
    connect(this, &QMenu::aboutToShow, this, &RemoteContentMenu::updateMenu);
}

RemoteContentMenu::~RemoteContentMenu() = default;

void RemoteContentMenu::slotConfigure()
{
    MessageViewer::RemoteContentConfigureDialog dlg(this);
    dlg.exec();
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
    for (auto act : std::as_const(mListAction)) {
        removeAction(act);
    }
    const auto numberOfUrl{mUrls.count()};
    if (numberOfUrl > 0) {
        mListAction.reserve(numberOfUrl + 3);
        for (const QString &url : std::as_const(mUrls)) {
            QAction *act = addAction(i18n("Authorize %1", url));
            connect(act, &QAction::triggered, this, [this, url]() {
                authorize(url);
                mUrls.removeAll(url);
            });
            mListAction << act;
        }
        mListAction << addSeparator();
        QAction *act = addAction(i18n("Authorize all Urls"));
        connect(act, &QAction::triggered, this, [this]() {
            for (const QString &url : std::as_const(mUrls)) {
                authorize(url);
            }
            mUrls.clear();
        });
        mListAction << act;
        mListAction << addSeparator();
    }
    addAction(mConfigureRemoteContentAction);
    connect(mConfigureRemoteContentAction, &QAction::triggered, this, &RemoteContentMenu::slotConfigure);
}

void RemoteContentMenu::authorize(const QString &url)
{
    RemoteContentInfo info;
    info.setUrl(url);
    info.setStatus(RemoteContentInfo::RemoteContentInfoStatus::Authorized);
    RemoteContentManager::self()->addRemoteContent(info);
    Q_EMIT updateEmail();
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

#include "moc_remotecontentmenu.cpp"
