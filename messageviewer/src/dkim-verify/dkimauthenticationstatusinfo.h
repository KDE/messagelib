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

#ifndef DKIMAUTHENTICATIONSTATUSINFO_H
#define DKIMAUTHENTICATIONSTATUSINFO_H

#include "messageviewer_private_export.h"
#include <QString>
#include <QDebug>
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMAuthenticationStatusInfo
{
public:
    DKIMAuthenticationStatusInfo();

    struct AuthStatusInfo {
        QString method;
        QString result;
        QString reason;
        int methodVersion = -1;
        //TODO add more
        bool operator==(const AuthStatusInfo &other) const;
    };

    Q_REQUIRED_RESULT QString authservId() const;
    void setAuthservId(const QString &authservId);

    Q_REQUIRED_RESULT bool parseAuthenticationStatus(const QString &key);

    Q_REQUIRED_RESULT int authVersion() const;
    void setAuthVersion(int authVersion);

    Q_REQUIRED_RESULT QString reasonSpec() const;
    void setReasonSpec(const QString &reasonSpec);

    Q_REQUIRED_RESULT bool operator==(const DKIMAuthenticationStatusInfo &other) const;

    Q_REQUIRED_RESULT QList<AuthStatusInfo> listAuthStatusInfo() const;
    void setListAuthStatusInfo(const QList<AuthStatusInfo> &listAuthStatusInfo);

private:
    AuthStatusInfo parseAuthResultInfo(QString &valueKey);
    QList<AuthStatusInfo> mListAuthStatusInfo;
    QString mAuthservId;
    QString mReasonSpec;
    int mAuthVersion = -1;
};
}

Q_DECLARE_METATYPE(MessageViewer::DKIMAuthenticationStatusInfo)
MESSAGEVIEWER_EXPORT QDebug operator <<(QDebug d, const MessageViewer::DKIMAuthenticationStatusInfo &t);

#endif // DKIMAUTHENTICATIONSTATUSINFO_H
