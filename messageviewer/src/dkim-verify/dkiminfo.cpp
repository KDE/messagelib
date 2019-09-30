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

#include "dkiminfo.h"
#include "messageviewer_debug.h"

#include <QStringList>
#include "messageviewer_debug.h"
using namespace MessageViewer;

DKIMInfo::DKIMInfo()
{
}

void DKIMInfo::parseDKIM(const QString &header)
{
    if (header.isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Error: trying to parse empty header";
        //TODO error ?
        return;
    }
    const QStringList items = header.split(QLatin1Char(';'));
    for (int i = 0; i < items.count(); ++i) {
        const QString elem = items.at(i).trimmed();
        if (elem.startsWith(QLatin1String("v="))) {
            mVersion = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("a="))) {
            mSigningAlgorithm = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("t="))) {
            mSignatureTimeStamp = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("c="))) {

        } else if (elem.startsWith(QLatin1String("l="))) {
            mBodyLenghtCount = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("i="))) {
            mUserAgent = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("q="))) {
            mQuery = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("d="))) {
            mDomain = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("s="))) {
            mSelector = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("b="))) {
            mSignature = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("h="))) {
            //mListSignedHeader = elem.right(elem.length() - 2);
            //TODO parse elements.
            //mHashingAlgorithm = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("hb="))) {
        } else {
            qCWarning(MESSAGEVIEWER_LOG) << " Unknown element type" << elem;
        }
    }
    //TODO
}

QString DKIMInfo::version() const
{
    return mVersion;
}

void DKIMInfo::setVersion(const QString &version)
{
    mVersion = version;
}

QString DKIMInfo::hashingAlgorithm() const
{
    return mHashingAlgorithm;
}

void DKIMInfo::setHashingAlgorithm(const QString &hashingAlgorithm)
{
    mHashingAlgorithm = hashingAlgorithm;
}

QString DKIMInfo::domain() const
{
    return mDomain;
}

void DKIMInfo::setDomain(const QString &domain)
{
    mDomain = domain;
}

QString DKIMInfo::selector() const
{
    return mSelector;
}

void DKIMInfo::setSelector(const QString &selector)
{
    mSelector = selector;
}

QString DKIMInfo::bodyHash() const
{
    return mBodyHash;
}

void DKIMInfo::setBodyHash(const QString &bodyHash)
{
    mBodyHash = bodyHash;
}

bool DKIMInfo::isValid() const
{
    //TODO verify

    return !mSelector.isEmpty() && !mDomain.isEmpty() && !mBodyHash.isEmpty() && !mHashingAlgorithm.isEmpty();
}

QStringList DKIMInfo::listSignedHeader() const
{
    return mListSignedHeader;
}

void DKIMInfo::setListSignedHeader(const QStringList &listSignedHeader)
{
    mListSignedHeader = listSignedHeader;
}

QString DKIMInfo::signingAlgorithm() const
{
    return mSigningAlgorithm;
}

void DKIMInfo::setSigningAlgorithm(const QString &signingAlgorithm)
{
    mSigningAlgorithm = signingAlgorithm;
}

QString DKIMInfo::signatureTimeStamp() const
{
    return mSignatureTimeStamp;
}

void DKIMInfo::setSignatureTimeStamp(const QString &signatureTimeStamp)
{
    mSignatureTimeStamp = signatureTimeStamp;
}

QString DKIMInfo::query() const
{
    return mQuery;
}

void DKIMInfo::setQuery(const QString &query)
{
    mQuery = query;
}

QString DKIMInfo::expireTime() const
{
    return mExpireTime;
}

void DKIMInfo::setExpireTime(const QString &expireTime)
{
    mExpireTime = expireTime;
}

QString DKIMInfo::signature() const
{
    return mSignature;
}

void DKIMInfo::setSignature(const QString &signature)
{
    mSignature = signature;
}

QString DKIMInfo::userAgent() const
{
    return mUserAgent;
}

void DKIMInfo::setUserAgent(const QString &userAgent)
{
    mUserAgent = userAgent;
}

QString DKIMInfo::bodyLenghtCount() const
{
    return mBodyLenghtCount;
}

void DKIMInfo::setBodyLenghtCount(const QString &bodyLenghtCount)
{
    mBodyLenghtCount = bodyLenghtCount;
}
