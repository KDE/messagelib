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

#include "dmarcrecordjob.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QDnsLookup>
using namespace MessageViewer;
//see https://tools.ietf.org/html/rfc7489
DMARCRecordJob::DMARCRecordJob(QObject *parent)
    : QObject(parent)
{
}

DMARCRecordJob::~DMARCRecordJob()
{
}

bool DMARCRecordJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start download dmarc key.";
        deleteLater();
        return false;
    }
    mDnsLookup = new QDnsLookup(this);
    connect(mDnsLookup, &QDnsLookup::finished,
            this, &DMARCRecordJob::resolvDnsDone);

    mDnsLookup->setType(QDnsLookup::TXT);
    mDnsLookup->setName(resolvDnsValue());
    mDnsLookup->lookup();
    return true;
}

QString DMARCRecordJob::resolvDnsValue() const
{
    const QString name = QLatin1String("_dmarc.") + mDomainName;
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
