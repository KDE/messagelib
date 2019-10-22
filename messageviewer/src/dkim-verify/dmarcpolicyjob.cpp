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

#include "dmarcpolicyjob.h"
#include "dmarcrecordjob.h"
#include "dmarcinfo.h"
#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace MessageViewer;

DMARCPolicyJob::DMARCPolicyJob(QObject *parent)
    : QObject(parent)
{
}

DMARCPolicyJob::~DMARCPolicyJob()
{
}

bool DMARCPolicyJob::canStart() const
{
    return !mEmailAddress.isEmpty();
}

bool DMARCPolicyJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Impossible to start DMARCPolicyJob" << mEmailAddress;
        Q_EMIT result({});
        deleteLater();
        return false;
    }

    const QString emailDomainStr = emailDomain();
    DMARCRecordJob *job = new DMARCRecordJob(this);
    job->setDomainName(emailDomainStr);
    connect(job, &MessageViewer::DMARCRecordJob::success, this, &DMARCPolicyJob::slotCheckDomain);
    connect(job, &MessageViewer::DMARCRecordJob::error, this, [this](const QString &err, const QString &domainName) {
        qDebug() << "error: " << err << " domain " << domainName;
        //Verify subdomain ?
        checkSubDomain(domainName);
    });
    if (!job->start()) {
        Q_EMIT result({});
        deleteLater();
        return false;
    }
    return true;
}

QByteArray DMARCPolicyJob::generateDMARCFromList(const QList<QByteArray> &lst) const
{
    QByteArray ba;
    if (lst.count() != 1) {
        for (const QByteArray &b : lst) {
            ba += b;
        }
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "DMARCPolicyJob Key result has more that 1 element" << lst;
    } else {
        ba = lst.at(0);
    }
    return ba;
}

void DMARCPolicyJob::slotCheckSubDomain(const QList<QByteArray> &lst, const QString &domainName)
{
    const QByteArray ba = generateDMARCFromList(lst);
    DMARCInfo info;
    if (info.parseDMARC(QString::fromLocal8Bit(ba))) {
        if ((info.version() != QLatin1String("DMARC1")) ||  info.policy().isEmpty() || (info.percentage() > 100 || info.percentage() < 0)) {
            Q_EMIT result({});
            deleteLater();
            return;
        } else {
            DMARCPolicyJob::DMARCResult val;
            val.mAdkim = info.adkim();
            val.mPercentage = info.percentage();
            val.mPolicy = info.subDomainPolicy().isEmpty() ? info.policy() : info.subDomainPolicy();
            //TODO verify it !
            val.mDomain = domainName;
            val.mSource = domainName;
            Q_EMIT result(val);
            deleteLater();
            return;
        }
    }
    Q_EMIT result(DMARCResult());
}

void DMARCPolicyJob::checkSubDomain(const QString &domainName)
{
    const QString subDomain = emailSubDomain(domainName);
    if (subDomain != domainName) {
        DMARCRecordJob *job = new DMARCRecordJob(this);
        job->setDomainName(subDomain);
        connect(job, &MessageViewer::DMARCRecordJob::success, this, &DMARCPolicyJob::slotCheckSubDomain);
        connect(job, &MessageViewer::DMARCRecordJob::error, this, [this](const QString &err, const QString &domainName) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "error: " << err << " domain " << domainName;
            Q_EMIT result({});
            deleteLater();
        });
        if (!job->start()) {
            deleteLater();
            Q_EMIT result({});
            return;
        }
    } else {
        //Invalid
        Q_EMIT result({});
        deleteLater();
        return;
    }
}

void DMARCPolicyJob::slotCheckDomain(const QList<QByteArray> &lst, const QString &domainName)
{
    const QByteArray ba = generateDMARCFromList(lst);
    DMARCInfo info;
    if (info.parseDMARC(QString::fromLocal8Bit(ba))) {
        if ((info.version() != QLatin1String("DMARC1")) ||  info.policy().isEmpty()
                || (info.percentage() != -1 && (info.percentage() > 100 || info.percentage() < 0))) {
            //Invalid
            //Check subdomain
            checkSubDomain(domainName);
        } else {
            DMARCPolicyJob::DMARCResult val;
            val.mAdkim = info.adkim();
            val.mPercentage = info.percentage();
            val.mPolicy = info.policy();
            val.mDomain = domainName;
            val.mSource = domainName;
            Q_EMIT result(val);
            deleteLater();
            return;
        }
    } else {
        //Check subdomain
        checkSubDomain(domainName);
    }
    qDebug() << "domainName: " << domainName << " lst " << lst;
    //Parse result
}

QString DMARCPolicyJob::emailDomain() const
{
    return MessageViewer::DKIMUtil::emailDomain(mEmailAddress);
}

QString DMARCPolicyJob::emailSubDomain(const QString &domainName) const
{
    return MessageViewer::DKIMUtil::emailSubDomain(domainName);
}

QString DMARCPolicyJob::emailAddress() const
{
    return mEmailAddress;
}

void DMARCPolicyJob::setEmailAddress(const QString &emailAddress)
{
    mEmailAddress = emailAddress;
}
