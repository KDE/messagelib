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

#include "dkimkeyrecord.h"
#include "messageviewer_debug.h"

using namespace MessageViewer;

DKIMKeyRecord::DKIMKeyRecord()
{
}

bool DKIMKeyRecord::parseKey(const QString &key)
{
    if (key.isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Error: trying to parse empty key";
        return false;
    }
    const QStringList items = key.split(QLatin1String("; "));
    for (int i = 0; i < items.count(); ++i) {
        const QString elem = items.at(i).trimmed();
        if (elem.startsWith(QLatin1String("v="))) {
            mVersion = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("h="))) {
            //TODO
        } else if (elem.startsWith(QLatin1String("k="))) { //Key type (rsa by default)
            mKeyType = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("n="))) { //Notes (optional empty by default)
            mNote = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("p="))) { //Public key
            mPublicKey = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("s="))) { //Service Default is "*"
            mService = elem.right(elem.length() - 2); //TODO split it
        } else if (elem.startsWith(QLatin1String("t="))) { //Flag
            //TODO
        }
    }
    if (mVersion.isEmpty()) { //It's optional
        mVersion = QStringLiteral("DKIM1");
    }
    if (mKeyType.isEmpty()) { //Rsa by default
        mKeyType = QStringLiteral("rsa");
    }
    if (mService.isEmpty()) { //Rsa by default
        mKeyType = QLatin1Char('*');
    }
    return false;
}

QString DKIMKeyRecord::version() const
{
    return mVersion;
}

void DKIMKeyRecord::setVersion(const QString &version)
{
    mVersion = version;
}

QString DKIMKeyRecord::keyType() const
{
    return mKeyType;
}

void DKIMKeyRecord::setKeyType(const QString &keyType)
{
    mKeyType = keyType;
}

QString DKIMKeyRecord::note() const
{
    return mNote;
}

void DKIMKeyRecord::setNote(const QString &note)
{
    mNote = note;
}

QString DKIMKeyRecord::publicKey() const
{
    return mPublicKey;
}

void DKIMKeyRecord::setPublicKey(const QString &publicKey)
{
    mPublicKey = publicKey;
}

QString DKIMKeyRecord::service() const
{
    return mService;
}

void DKIMKeyRecord::setService(const QString &service)
{
    mService = service;
}
