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


#include "lookupkey.h"
#include <QDnsLookup>
#include <QDebug>
LookUpKey::LookUpKey(QObject *parent)
    : QObject(parent)
{
}

void LookUpKey::lookUpServer(const QString &addr)
{
    // Create a DNS lookup.
    mDnsLookup = new QDnsLookup(this);
    connect(mDnsLookup, &QDnsLookup::finished,
            this, &LookUpKey::handleServers);

    mDnsLookup->setType(QDnsLookup::TXT);
    //mDnsLookup->setName(QStringLiteral("s2048._domainkey.yahoo.fr"));
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
    for (const QDnsTextRecord &record : records) {
        qDebug() << " record " << record.values();
    }
    mDnsLookup->deleteLater();
}
