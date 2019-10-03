/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#include "dkimchecksignaturejob.h"
#include "dkiminfo.h"
#include "messageviewer_debug.h"
#include <KLocalizedString>
#include <QDateTime>

using namespace MessageViewer;
DKIMCheckSignatureJob::DKIMCheckSignatureJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckSignatureJob::~DKIMCheckSignatureJob()
{
}

void DKIMCheckSignatureJob::start()
{
    if (mDkimValue.isEmpty()) {
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned);
        deleteLater();
        return;
    }
    DKIMInfo info;
    if (!info.parseDKIM(mDkimValue)) {
        qCWarning(MESSAGEVIEWER_LOG) << "Impossible to parse header" << mDkimValue;
        deleteLater();
        return;
    }
    checkSignature(info);
    deleteLater();
}

void DKIMCheckSignatureJob::checkSignature(const DKIMInfo &info)
{
    const qint64 currentDate = QDateTime::currentSecsSinceEpoch();
    if (info.expireTime() != -1 && info.expireTime() < currentDate) {
        mWarningFound = i18n("Signature Expired");
    }
    if (info.signatureTimeStamp() != -1 && info.signatureTimeStamp() > currentDate) {
        mWarningFound += i18n("Signature created in the future");
    }
    if (info.signature().isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Signature doesn't exist";
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
        return;
    }
    //Add more test
    //TODO check if info is valid
}

QString DKIMCheckSignatureJob::warningFound() const
{
    return mWarningFound;
}

void DKIMCheckSignatureJob::setWarningFound(const QString &warningFound)
{
    mWarningFound = warningFound;
}

DKIMCheckSignatureJob::DKIMStatus DKIMCheckSignatureJob::status() const
{
    return mStatus;
}

void DKIMCheckSignatureJob::setStatus(const DKIMCheckSignatureJob::DKIMStatus &status)
{
    mStatus = status;
}

QString DKIMCheckSignatureJob::dkimValue() const
{
    return mDkimValue;
}

void DKIMCheckSignatureJob::setDkimValue(const QString &dkimValue)
{
    mDkimValue = dkimValue;
}
