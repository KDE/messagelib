/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcrecordjob.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QDnsLookup>

using namespace MessageViewer;
// see https://tools.ietf.org/html/rfc7489
DMARCRecordJob::DMARCRecordJob(QObject *parent)
    : QObject(parent)
{
}

DMARCRecordJob::~DMARCRecordJob() = default;

bool DMARCRecordJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start download dmarc key.";
        deleteLater();
        return false;
    }
    mDnsLookup = new QDnsLookup(this);
    connect(mDnsLookup, &QDnsLookup::finished, this, &DMARCRecordJob::resolvDnsDone);

    mDnsLookup->setType(QDnsLookup::TXT);
    mDnsLookup->setName(resolvDnsValue());
    mDnsLookup->lookup();
    return true;
}

QString DMARCRecordJob::resolvDnsValue() const
{
    const QString name = QLatin1StringView("_dmarc.") + mDomainName;
    qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "DMARCRecordJob::resolvDnsValue" << name;
    return name;
}

void DMARCRecordJob::resolvDnsDone()
{
    // Check the lookup succeeded.
    if (mDnsLookup->error() != QDnsLookup::NoError) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Error during resolving: " << mDnsLookup->errorString() << " mDnsLookup->error() " << mDnsLookup->error();
        Q_EMIT error(mDnsLookup->errorString(), mDomainName);
        deleteLater();
        return;
    }

    // Handle the results.
    const auto records = mDnsLookup->textRecords();
    QList<QByteArray> textRecordResult;
    textRecordResult.reserve(records.count());
    for (const QDnsTextRecord &record : records) {
        textRecordResult << record.values();
    }

    Q_EMIT success(textRecordResult, mDomainName);
    deleteLater();
}

bool DMARCRecordJob::canStart() const
{
    return !mDomainName.isEmpty();
}

QString DMARCRecordJob::domainName() const
{
    return mDomainName;
}

void DMARCRecordJob::setDomainName(const QString &domainName)
{
    mDomainName = domainName;
}

#include "moc_dmarcrecordjob.cpp"
