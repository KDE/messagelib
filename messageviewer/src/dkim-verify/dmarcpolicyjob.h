/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <QObject>
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DMARCPolicyJob : public QObject
{
    Q_OBJECT
public:
    struct DMARCResult {
        bool isValid() const
        {
            return !mAdkim.isEmpty() && !mPolicy.isEmpty() && !mDomain.isEmpty() && !mSource.isEmpty();
        }

        [[nodiscard]] bool operator==(const DMARCPolicyJob::DMARCResult &other) const
        {
            return mAdkim == other.mAdkim && mPolicy == other.mPolicy && mDomain == other.mDomain && mSource == other.mSource
                && mPercentage == other.mPercentage;
        }

        [[nodiscard]] bool operator!=(const DMARCPolicyJob::DMARCResult &other) const
        {
            return !DMARCResult::operator==(other);
        }

        QString mAdkim;
        QString mPolicy;
        QString mDomain;
        QString mSource;
        int mPercentage = -1;
    };

    explicit DMARCPolicyJob(QObject *parent = nullptr);
    ~DMARCPolicyJob() override;

    [[nodiscard]] bool canStart() const;
    [[nodiscard]] bool start();

    [[nodiscard]] QString emailAddress() const;
    void setEmailAddress(const QString &emailAddress);

Q_SIGNALS:
    void result(const MessageViewer::DMARCPolicyJob::DMARCResult &value, const QString &emailAddress);

private:
    MESSAGEVIEWER_NO_EXPORT void checkSubDomain(const QString &domainName);
    MESSAGEVIEWER_NO_EXPORT void slotCheckDomain(const QList<QByteArray> &lst, const QString &domainName);
    MESSAGEVIEWER_NO_EXPORT void slotCheckSubDomain(const QList<QByteArray> &lst, const QString &domainName);
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QByteArray generateDMARCFromList(const QList<QByteArray> &lst) const;
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString emailDomain() const;
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString emailSubDomain(const QString &domainName) const;
    QString mEmailAddress;
};
}
