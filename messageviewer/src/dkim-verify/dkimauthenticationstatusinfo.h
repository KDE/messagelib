/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <QDebug>
#include <QString>
namespace MessageViewer
{
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
                return other.type == type && other.value == value;
            }
        };

        QString method;
        QString result;
        QString reason;
        int methodVersion = -1;

        // Add Properties
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
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMAuthenticationStatusInfo &t);

