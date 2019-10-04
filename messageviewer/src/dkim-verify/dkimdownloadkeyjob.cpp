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

#include "dkimdownloadkeyjob.h"
#include "messageviewer_debug.h"

#include <QDnsLookup>
using namespace MessageViewer;
DKIMDownloadKeyJob::DKIMDownloadKeyJob(QObject *parent)
    : QObject(parent)
{
}

DKIMDownloadKeyJob::~DKIMDownloadKeyJob()
{
}

bool DKIMDownloadKeyJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Impossible to start download public keys";
        deleteLater();
        return false;
    }
    mDnsLookup = new QDnsLookup(this);
    connect(mDnsLookup, &QDnsLookup::finished,
            this, &DKIMDownloadKeyJob::resolvDnsDone);

    mDnsLookup->setType(QDnsLookup::TXT);
    mDnsLookup->setName(resolvDnsValue());
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
    return mSelectorName + QLatin1String("._domainkey.") + mDomainName;
}

QDnsLookup *DKIMDownloadKeyJob::dnsLookup() const
{
    return mDnsLookup;
}

void DKIMDownloadKeyJob::resolvDnsDone()
{
    // Check the lookup succeeded.
    if (mDnsLookup->error() != QDnsLookup::NoError) {
        qCWarning(MESSAGEVIEWER_LOG) << "Error during resolving: " << mDnsLookup->errorString();
        Q_EMIT error();
        deleteLater();
        return;
    }

    // Handle the results.
    const auto records = mDnsLookup->textRecords();
    QList<QByteArray> textRecordResult;
    for (const QDnsTextRecord &record : records) {
        textRecordResult << record.values();
    }

    Q_EMIT success(textRecordResult);
    deleteLater();
}
