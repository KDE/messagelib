/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

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
            [[nodiscard]] bool isValid() const
            {
                return !type.isEmpty() && !value.isEmpty();
            }

            [[nodiscard]] bool operator==(const Property &other) const
            {
                return other.type == type && other.value == value;
            }
        };

        QString method;
        QString result;
        QString reason;
        int methodVersion = -1;

        // Add Properties
        QList<Property> smtp;
        QList<Property> header;
        QList<Property> body;
        QList<Property> policy;

        [[nodiscard]] bool operator==(const AuthStatusInfo &other) const;
        [[nodiscard]] bool isValid() const;
    };

    [[nodiscard]] QString authservId() const;
    void setAuthservId(const QString &authservId);

    [[nodiscard]] bool parseAuthenticationStatus(const QString &key, bool relaxingParsing);

    [[nodiscard]] int authVersion() const;
    void setAuthVersion(int authVersion);

    [[nodiscard]] QString reasonSpec() const;
    void setReasonSpec(const QString &reasonSpec);

    [[nodiscard]] bool operator==(const DKIMAuthenticationStatusInfo &other) const;

    [[nodiscard]] QList<AuthStatusInfo> listAuthStatusInfo() const;
    void setListAuthStatusInfo(const QList<AuthStatusInfo> &listAuthStatusInfo);

private:
    [[nodiscard]] AuthStatusInfo parseAuthResultInfo(QString &valueKey, bool relaxingParsing);
    QList<AuthStatusInfo> mListAuthStatusInfo;
    QString mAuthservId;
    QString mReasonSpec;
    int mAuthVersion = -1;
};
}
Q_DECLARE_METATYPE(MessageViewer::DKIMAuthenticationStatusInfo)
Q_DECLARE_TYPEINFO(MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo, Q_RELOCATABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMAuthenticationStatusInfo &t);
