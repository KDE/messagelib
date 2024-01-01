/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "lookupkey.h"
#include <QDebug>
#include <QDnsLookup>
LookUpKey::LookUpKey(QObject *parent)
    : QObject(parent)
{
}

void LookUpKey::lookUpServer(const QString &addr)
{
    // Create a DNS lookup.
    mAddress = addr;
    mDnsLookup = new QDnsLookup(this);
    connect(mDnsLookup, &QDnsLookup::finished, this, &LookUpKey::handleServers);

    mDnsLookup->setType(QDnsLookup::TXT);
    // mDnsLookup->setName(QStringLiteral("s2048._domainkey.yahoo.fr"));
    mDnsLookup->setName(addr);
    mDnsLookup->lookup();
}

void LookUpKey::handleServers()
{
    // Check the lookup succeeded.
    if (mDnsLookup->error() != QDnsLookup::NoError) {
        qWarning("DNS lookup failed");
        mDnsLookup->deleteLater();
        return;
    }

    // Handle the results.
    const auto records = mDnsLookup->textRecords();
    qDebug() << " resolve address " << mAddress;
    for (const QDnsTextRecord &record : records) {
        qDebug() << " record " << record.values();
    }
    mDnsLookup->deleteLater();
}

#include "moc_lookupkey.cpp"
