/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckauthenticationstatusjob.h"
#include "dkimauthenticationstatusinfo.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <KMime/Message>
using namespace MessageViewer;
// see https://tools.ietf.org/html/rfc7601
DKIMCheckAuthenticationStatusJob::DKIMCheckAuthenticationStatusJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckAuthenticationStatusJob::~DKIMCheckAuthenticationStatusJob() = default;

void DKIMCheckAuthenticationStatusJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start job";
        deleteLater();
        Q_EMIT result({});
        return;
    }

    const QString strAuthenticationHeader = QStringLiteral("authentication-results");
    QString str = mHeaderParser.headerType(strAuthenticationHeader);
    DKIMAuthenticationStatusInfo info;
    while (!str.isEmpty()) {
        if (!info.parseAuthenticationStatus(str, mUseRelaxedParsing)) {
            break;
        }
        str = mHeaderParser.headerType(strAuthenticationHeader);
    }

    // qDebug() << "result info: " << info;
    Q_EMIT result(info);
    deleteLater();
}

bool DKIMCheckAuthenticationStatusJob::canStart() const
{
    return mHeaderParser.wasAlreadyParsed();
}

void DKIMCheckAuthenticationStatusJob::setHeaderParser(const DKIMHeaderParser &headerParser)
{
    mHeaderParser = headerParser;
}

bool DKIMCheckAuthenticationStatusJob::useRelaxedParsing() const
{
    return mUseRelaxedParsing;
}

void DKIMCheckAuthenticationStatusJob::setUseRelaxedParsing(bool useRelaxedParsing)
{
    mUseRelaxedParsing = useRelaxedParsing;
}

#include "moc_dkimcheckauthenticationstatusjob.cpp"
