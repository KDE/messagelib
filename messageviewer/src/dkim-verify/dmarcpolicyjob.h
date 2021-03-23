/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dmarcinfo.h"
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

        Q_REQUIRED_RESULT bool operator==(const DMARCPolicyJob::DMARCResult &other) const
        {
            return mAdkim == other.mAdkim && mPolicy == other.mPolicy && mDomain == other.mDomain && mSource == other.mSource
                && mPercentage == other.mPercentage;
        }

        Q_REQUIRED_RESULT bool operator!=(const DMARCPolicyJob::DMARCResult &other) const
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

    Q_REQUIRED_RESULT bool canStart() const;
    Q_REQUIRED_RESULT bool start();

    Q_REQUIRED_RESULT QString emailAddress() const;
    void setEmailAddress(const QString &emailAddress);

Q_SIGNALS:
    void result(const MessageViewer::DMARCPolicyJob::DMARCResult &value, const QString &emailAddress);

private:
    void checkSubDomain(const QString &domainName);
    void slotCheckDomain(const QList<QByteArray> &lst, const QString &domainName);
    void slotCheckSubDomain(const QList<QByteArray> &lst, const QString &domainName);
    Q_REQUIRED_RESULT QByteArray generateDMARCFromList(const QList<QByteArray> &lst) const;
    Q_REQUIRED_RESULT QString emailDomain() const;
    Q_REQUIRED_RESULT QString emailSubDomain(const QString &domainName) const;
    QString mEmailAddress;
};
}
