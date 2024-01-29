/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimdownloadkeyjob.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QDnsLookup>
using namespace MessageViewer;
DKIMDownloadKeyJob::DKIMDownloadKeyJob(QObject *parent)
    : QObject(parent)
{
}

DKIMDownloadKeyJob::~DKIMDownloadKeyJob() = default;

bool DKIMDownloadKeyJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start download public keys";
        deleteLater();
        return false;
    }
    mDnsLookup = new QDnsLookup(this);
    connect(mDnsLookup, &QDnsLookup::finished, this, &DKIMDownloadKeyJob::resolvDnsDone);

    mDnsLookup->setType(QDnsLookup::TXT);
    mDnsLookup->setName(resolvDnsValue());
    mDnsLookup->lookup();
    return true;
}

bool DKIMDownloadKeyJob::canStart() const
{
    return !mDomainName.isEmpty() && !mSelectorName.isEmpty();
}

QString DKIMDownloadKeyJob::domainName() const
{
    return mDomainName;
}

void DKIMDownloadKeyJob::setDomainName(const QString &domainName)
{
    mDomainName = domainName;
}

QString DKIMDownloadKeyJob::selectorName() const
{
    return mSelectorName;
}

void DKIMDownloadKeyJob::setSelectorName(const QString &selectorName)
{
    mSelectorName = selectorName;
}

QString DKIMDownloadKeyJob::resolvDnsValue() const
{
    const QString name = mSelectorName + QLatin1StringView("._domainkey.") + mDomainName;
    qDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "DKIMDownloadKeyJob::resolvDnsValue" << name;
    return name;
}

QDnsLookup *DKIMDownloadKeyJob::dnsLookup() const
{
    return mDnsLookup;
}

void DKIMDownloadKeyJob::resolvDnsDone()
{
    // Check the lookup succeeded.
    if (mDnsLookup->error() != QDnsLookup::NoError) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Error during resolving: " << mDnsLookup->errorString();
        Q_EMIT error(mDnsLookup->errorString());
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

    Q_EMIT success(textRecordResult, mDomainName, mSelectorName);
    deleteLater();
}

#include "moc_dkimdownloadkeyjob.cpp"
