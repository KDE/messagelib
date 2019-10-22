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
#include "messageviewer_dkimcheckerdebug.h"

using namespace MessageViewer;

DKIMKeyRecord::DKIMKeyRecord()
{
}

bool DKIMKeyRecord::parseKey(const QString &key)
{
    QString newKey = key;
    if (newKey.isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Error: trying to parse empty key";
        return false;
    }
    newKey.replace(QLatin1String("; "), QLatin1String(";"));
    const QStringList items = newKey.split(QLatin1Char(';'));
    for (int i = 0; i < items.count(); ++i) {
        const QString elem = items.at(i).trimmed();
        if (elem.startsWith(QLatin1String("v="))) {
            mVersion = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("h="))) {
            //Parse multi array.
            mHashAlgorithm = elem.right(elem.length() - 2).split(QLatin1Char(':'));
        } else if (elem.startsWith(QLatin1String("k="))) { //Key type (rsa by default)
            mKeyType = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("n="))) { //Notes (optional empty by default)
            mNote = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("p="))) { //Public key
            mPublicKey = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("s="))) { //Service Default is "*"
            //Service Type (plain-text; OPTIONAL; default is "*").  A colon-
            //separated list of service types to which this record applies.
            //Verifiers for a given service type MUST ignore this record if the
            //appropriate type is not listed.  Unrecognized service types MUST
            //be ignored.  Currently defined service types are as follows:
            const QStringList lst = elem.right(elem.length() - 2).split(QLatin1Char(':'));
            for (const QString &service : lst) {
                if (service == QLatin1Char('*') || service == QLatin1String("email")) {
                    mService = service;
                }
            }
        } else if (elem.startsWith(QLatin1String("t="))) { //Flag
            //            t= Flags, represented as a colon-separated list of names (plain-
            //                  text; OPTIONAL, default is no flags set).  Unrecognized flags MUST
            //                  be ignored.  The defined flags are as follows:


            //TODO a option for disable "y" when we want.

            //                  y  This domain is testing DKIM.  Verifiers MUST NOT treat messages
            //                     from Signers in testing mode differently from unsigned email,
            //                     even should the signature fail to verify.  Verifiers MAY wish
            //                     to track testing mode results to assist the Signer.

            //                  s  Any DKIM-Signature header fields using the "i=" tag MUST have
            //                     the same domain value on the right-hand side of the "@" in the
            //                     "i=" tag and the value of the "d=" tag.  That is, the "i="
            //                     domain MUST NOT be a subdomain of "d=".  Use of this flag is
            //                     RECOMMENDED unless subdomaining is required.
            mFlags = elem.right(elem.length() - 2).split(QLatin1Char(':'));
        }
    }
    if (mVersion.isEmpty()) { //It's optional
        mVersion = QStringLiteral("DKIM1");
    }
    if (mKeyType.isEmpty()) { //Rsa by default
        mKeyType = QStringLiteral("rsa");
    }
    if (mService.isEmpty()) {
        mService = QLatin1Char('*');
    }
    return true;
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

QStringList DKIMKeyRecord::flags() const
{
    return mFlags;
}

void DKIMKeyRecord::setFlags(const QStringList &flags)
{
    mFlags = flags;
}

bool DKIMKeyRecord::operator==(const DKIMKeyRecord &other) const
{
    return mVersion == other.version()
           && mNote == other.note()
           && mPublicKey == other.publicKey()
           && mService == other.service()
           && mHashAlgorithm == other.hashAlgorithm()
           && mFlags == other.flags();
}

QStringList DKIMKeyRecord::hashAlgorithm() const
{
    return mHashAlgorithm;
}

void DKIMKeyRecord::setHashAlgorithm(const QStringList &hashAlgorithm)
{
    mHashAlgorithm = hashAlgorithm;
}

QDebug operator <<(QDebug d, const DKIMKeyRecord &t)
{
    d << "mVersion " << t.version();
    d << "mKeyType " << t.keyType();
    d << "mNote " << t.note();
    d << "mPublicKey " << t.publicKey();
    d << "mService " << t.service();
    d << "mHashAlgorithm " << t.hashAlgorithm();
    d << "mFlags " << t.flags();
    return d;
}
