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

#ifndef DKIMCHECKSIGNATUREJOB_H
#define DKIMCHECKSIGNATUREJOB_H

#include <QObject>
#include "messageviewer_export.h"
namespace MessageViewer {
class DKIMInfo;
class MESSAGEVIEWER_EXPORT DKIMCheckSignatureJob : public QObject
{
    Q_OBJECT
public:
    enum class DKIMStatus {
        Unknown,
        Valid,
        Invalid,
        EmailNotSigned
    };

    enum class DKIMError {
        Any,
        CorruptedBodyHash,
        DomainNotExist,
        MissingFrom,
        MissingSignature,
        InvalidQueryMethod,
        InvalidHeaderCanonicalization,
        InvalidBodyCanonicalization,
        //TODO add more
    };

    explicit DKIMCheckSignatureJob(QObject *parent = nullptr);
    ~DKIMCheckSignatureJob();
    void start();

    Q_REQUIRED_RESULT QString dkimValue() const;
    void setDkimValue(const QString &dkimValue);

    Q_REQUIRED_RESULT DKIMCheckSignatureJob::DKIMStatus status() const;
    void setStatus(const DKIMCheckSignatureJob::DKIMStatus &status);

    Q_REQUIRED_RESULT QString warningFound() const;
    void setWarningFound(const QString &warningFound);
    Q_REQUIRED_RESULT MessageViewer::DKIMCheckSignatureJob::DKIMStatus checkSignature(const MessageViewer::DKIMInfo &info);

    Q_REQUIRED_RESULT DKIMCheckSignatureJob::DKIMError error() const;

Q_SIGNALS:
    void result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus status);

private:
    //TODO add message ?
    QString mDkimValue;
    QString mWarningFound;
    DKIMCheckSignatureJob::DKIMError mError = DKIMCheckSignatureJob::DKIMError::Any;
    DKIMCheckSignatureJob::DKIMStatus mStatus = DKIMCheckSignatureJob::DKIMStatus::Unknown;
};
}

#endif // DKIMCHECKSIGNATUREJOB_H
