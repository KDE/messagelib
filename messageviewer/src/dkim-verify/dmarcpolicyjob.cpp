/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcpolicyjob.h"
#include "dkimutil.h"
#include "dmarcinfo.h"
#include "dmarcrecordjob.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace MessageViewer;

DMARCPolicyJob::DMARCPolicyJob(QObject *parent)
    : QObject(parent)
{
}

DMARCPolicyJob::~DMARCPolicyJob() = default;

bool DMARCPolicyJob::canStart() const
{
    return !mEmailAddress.isEmpty();
}

bool DMARCPolicyJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Impossible to start DMARCPolicyJob" << mEmailAddress;
        Q_EMIT result({}, mEmailAddress);
        deleteLater();
        return false;
    }

    auto job = new DMARCRecordJob(this);
    job->setDomainName(emailDomain());
    connect(job, &MessageViewer::DMARCRecordJob::success, this, &DMARCPolicyJob::slotCheckDomain);
    connect(job, &MessageViewer::DMARCRecordJob::error, this, [this](const QString &err, const QString &domainName) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "error: " << err << " domain " << domainName;
        // Verify subdomain
        checkSubDomain(domainName);
    });
    if (!job->start()) {
        Q_EMIT result({}, mEmailAddress);
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
        if ((info.version() != QLatin1StringView("DMARC1")) || info.policy().isEmpty() || (info.percentage() > 100 || info.percentage() < 0)) {
            Q_EMIT result({}, mEmailAddress);
            deleteLater();
            return;
        } else {
            DMARCPolicyJob::DMARCResult val;
            val.mAdkim = info.adkim();
            val.mPercentage = info.percentage();
            val.mPolicy = info.subDomainPolicy().isEmpty() ? info.policy() : info.subDomainPolicy();
            // TODO verify it !
            val.mDomain = domainName;
            val.mSource = domainName;
            Q_EMIT result(val, mEmailAddress);
            deleteLater();
            return;
        }
    }
    Q_EMIT result({}, mEmailAddress);
}

void DMARCPolicyJob::checkSubDomain(const QString &domainName)
{
    const QString subDomain = emailSubDomain(domainName);
    if (subDomain != domainName) {
        auto job = new DMARCRecordJob(this);
        job->setDomainName(subDomain);
        connect(job, &MessageViewer::DMARCRecordJob::success, this, &DMARCPolicyJob::slotCheckSubDomain);
        connect(job, &MessageViewer::DMARCRecordJob::error, this, [this](const QString &err, const QString &domainName) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "error: " << err << " domain " << domainName;
            Q_EMIT result({}, mEmailAddress);
            deleteLater();
        });
        if (!job->start()) {
            Q_EMIT result({}, mEmailAddress);
            deleteLater();
            return;
        }
    } else {
        // Invalid
        Q_EMIT result({}, mEmailAddress);
        deleteLater();
        return;
    }
}

void DMARCPolicyJob::slotCheckDomain(const QList<QByteArray> &lst, const QString &domainName)
{
    const QByteArray ba = generateDMARCFromList(lst);
    DMARCInfo info;
    if (info.parseDMARC(QString::fromLocal8Bit(ba))) {
        if ((info.version() != QLatin1StringView("DMARC1")) || info.policy().isEmpty()
            || (info.percentage() != -1 && (info.percentage() > 100 || info.percentage() < 0))) {
            // Invalid
            // Check subdomain
            checkSubDomain(domainName);
        } else {
            DMARCPolicyJob::DMARCResult val;
            val.mAdkim = info.adkim();
            val.mPercentage = info.percentage();
            val.mPolicy = info.policy();
            val.mDomain = domainName;
            val.mSource = domainName;
            Q_EMIT result(val, mEmailAddress);
            deleteLater();
            return;
        }
    } else {
        // Check subdomain
        checkSubDomain(domainName);
    }
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

#include "moc_dmarcpolicyjob.cpp"
