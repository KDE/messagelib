/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimcheckauthenticationstatusjob.h"
#include "dkimauthenticationstatusinfo.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <KMime/Message>
using namespace MessageViewer;
//see https://tools.ietf.org/html/rfc7601
DKIMCheckAuthenticationStatusJob::DKIMCheckAuthenticationStatusJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckAuthenticationStatusJob::~DKIMCheckAuthenticationStatusJob()
{
}

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

    //qDebug() << "result info: " << info;
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
