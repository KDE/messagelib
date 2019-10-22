/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#ifndef DMARCPOLICYJOB_H
#define DMARCPOLICYJOB_H

#include <QObject>
#include "dmarcinfo.h"
#include "messageviewer_private_export.h"
namespace MessageViewer {
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
            return mAdkim == other.mAdkim && mPolicy == other.mPolicy && mDomain == other.mDomain && mSource == other.mSource && mPercentage == other.mPercentage;
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
    ~DMARCPolicyJob();

    Q_REQUIRED_RESULT bool canStart() const;
    Q_REQUIRED_RESULT bool start();

    Q_REQUIRED_RESULT QString emailAddress() const;
    void setEmailAddress(const QString &emailAddress);

Q_SIGNALS:
    void result(const MessageViewer::DMARCPolicyJob::DMARCResult &value);

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
#endif // DMARCPOLICYJOB_H
