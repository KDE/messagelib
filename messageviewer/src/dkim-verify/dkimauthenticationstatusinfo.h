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
        struct Property {
            QString type;
            QString value;
            Q_REQUIRED_RESULT bool isValid() const
            {
                return !type.isEmpty() && !value.isEmpty();
            }

            Q_REQUIRED_RESULT bool operator==(const Property &other) const
            {
                return other.type == type
                       && other.value == value;
            }
        };

        QString method;
        QString result;
        QString reason;
        int methodVersion = -1;

        //Add Properties
        QVector<Property> smtp;
        QVector<Property> header;
        QVector<Property> body;
        QVector<Property> policy;

        Q_REQUIRED_RESULT bool operator==(const AuthStatusInfo &other) const;
        Q_REQUIRED_RESULT bool isValid() const;
    };

    Q_REQUIRED_RESULT QString authservId() const;
    void setAuthservId(const QString &authservId);

    Q_REQUIRED_RESULT bool parseAuthenticationStatus(const QString &key, bool relaxingParsing);

    Q_REQUIRED_RESULT int authVersion() const;
    void setAuthVersion(int authVersion);

    Q_REQUIRED_RESULT QString reasonSpec() const;
    void setReasonSpec(const QString &reasonSpec);

    Q_REQUIRED_RESULT bool operator==(const DKIMAuthenticationStatusInfo &other) const;

    Q_REQUIRED_RESULT QVector<AuthStatusInfo> listAuthStatusInfo() const;
    void setListAuthStatusInfo(const QVector<AuthStatusInfo> &listAuthStatusInfo);

private:
    Q_REQUIRED_RESULT AuthStatusInfo parseAuthResultInfo(QString &valueKey, bool relaxingParsing);
    QVector<AuthStatusInfo> mListAuthStatusInfo;
    QString mAuthservId;
    QString mReasonSpec;
    int mAuthVersion = -1;
};
}
Q_DECLARE_METATYPE(MessageViewer::DKIMAuthenticationStatusInfo)
Q_DECLARE_TYPEINFO(MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo, Q_MOVABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator <<(QDebug d, const MessageViewer::DKIMAuthenticationStatusInfo &t);

#endif // DKIMAUTHENTICATIONSTATUSINFO_H
