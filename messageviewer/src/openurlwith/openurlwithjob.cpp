/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithjob.h"
#include "messageviewer_debug.h"
#include <KIO/CommandLauncherJob>
#include <KMacroExpander>
#include <KNotificationJobUiDelegate>
#include <KShell>
using namespace MessageViewer;
OpenUrlWithJob::OpenUrlWithJob(QObject *parent)
    : QObject{parent}
{
}

OpenUrlWithJob::~OpenUrlWithJob() = default;

bool OpenUrlWithJob::canStart() const
{
    return mInfo.isValid() && !mUrl.isEmpty();
}

void OpenUrlWithJob::start()
{
    if (!canStart()) {
        deleteLater();
        qCWarning(MESSAGEVIEWER_LOG) << " Impossible to start OpenUrlWithJob";
        return;
    }
    QHash<QChar, QString> map;
    map.insert(QLatin1Char('u'), mUrl.toString());

    const QString commandLine = mInfo.commandLine();
    const QString cmd = KMacroExpander::expandMacrosShellQuote(commandLine, map);
    const QStringList arg = KShell::splitArgs(cmd);
    // qDebug() << " cmd " << cmd << " arg " << arg << " mInfo.command() " << mInfo.command();
    auto job = new KIO::CommandLauncherJob(mInfo.command(), QStringList() << arg);
    job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
    job->start();
    deleteLater();
}

const OpenWithUrlInfo &OpenUrlWithJob::info() const
{
    return mInfo;
}

void OpenUrlWithJob::setInfo(const OpenWithUrlInfo &newInfo)
{
    mInfo = newInfo;
}

void OpenUrlWithJob::setUrl(const QUrl &url)
{
    mUrl = url;
}

#include "moc_openurlwithjob.cpp"
