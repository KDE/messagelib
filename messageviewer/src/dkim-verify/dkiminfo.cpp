/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

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
#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QRegularExpressionMatch>
#include <QStringList>
using namespace MessageViewer;

DKIMInfo::DKIMInfo()
{
}

bool DKIMInfo::parseDKIM(const QString &header)
{
    if (header.isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Error: trying to parse empty header";
        return false;
    }
    QString newHeaders = header;
    newHeaders.replace(QLatin1String("; "), QLatin1String(";"));
    const QStringList items = newHeaders.split(QLatin1Char(';'), QString::SkipEmptyParts);
    bool foundCanonizations = false;
    for (int i = 0; i < items.count(); ++i) {
        const QString elem = items.at(i).trimmed();
        if (elem.startsWith(QLatin1String("v="))) {
            mVersion = elem.right(elem.length() - 2).toInt();
            if (mVersion != 1) {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Version is not correct " << mVersion;
            }
        } else if (elem.startsWith(QLatin1String("a="))) {
            //Parse it as "algorithm.signature-algorithm.hash
            parseAlgorithm(elem.right(elem.length() - 2));
        } else if (elem.startsWith(QLatin1String("t="))) {
            mSignatureTimeStamp = elem.right(elem.length() - 2).toLong();
        } else if (elem.startsWith(QLatin1String("c="))) {
            //Parse header/body canonicalization (example c=relaxed/simple) only relaxed and simple.
            parseCanonicalization(elem.right(elem.length() - 2));
            foundCanonizations = true;
        } else if (elem.startsWith(QLatin1String("bh="))) {
            mBodyHash = elem.right(elem.length() - 3).remove(QLatin1Char(' '));
        } else if (elem.startsWith(QLatin1String("l="))) {
            mBodyLengthCount = elem.right(elem.length() - 2).toInt();
        } else if (elem.startsWith(QLatin1String("i="))) {
            mAgentOrUserIdentifier = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("q="))) {
            mQuery = elem.right(elem.length() - 2);
            if (mQuery != QLatin1String("dns/txt")) {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Query is not correct and not supported " << mQuery;
            }
        } else if (elem.startsWith(QLatin1String("d="))) {
            mDomain = elem.right(elem.length() - 2).trimmed();
        } else if (elem.startsWith(QLatin1String("s="))) {
            mSelector = elem.right(elem.length() - 2).trimmed();
        } else if (elem.startsWith(QLatin1String("b="))) {
            mSignature = elem.right(elem.length() - 2);
        } else if (elem.startsWith(QLatin1String("h="))) {
            const QString str = MessageViewer::DKIMUtil::cleanString(elem.right(elem.length() - 2));
            mListSignedHeader = str.split(QLatin1Char(':'));
        } else if (elem.startsWith(QLatin1String("x="))) {
            mExpireTime = elem.right(elem.length() - 2).toLong();
        } else if (elem.startsWith(QLatin1String("z="))) {
            mCopiedHeaderField = elem.right(elem.length() - 2).split(QLatin1Char(':'));
        } else {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Unknown element type" << elem << " : items : " << items;
        }
    }
    if (!foundCanonizations) { //Default
        mHeaderCanonization = Simple;
        mBodyCanonization = Simple;
    }
    if (mVersion == -1) {
        mVersion = 1;
    }
    if (mQuery.isEmpty()) {
        mQuery = QLatin1String("dns/txt");
    }
    if (mAgentOrUserIdentifier.isEmpty()) {
        mAgentOrUserIdentifier = QLatin1Char('@') + mDomain;
        mIDomain = mDomain;
    } else {
        const QStringList lst = mAgentOrUserIdentifier.split(QLatin1Char('@'));
        if (lst.count() == 2) {
            if (mAgentOrUserIdentifier.isEmpty()) {
                mAgentOrUserIdentifier = QLatin1Char('@') + mDomain;
            }
            mIDomain = lst.at(1);
        }
    }
    return true;
}

void DKIMInfo::parseAlgorithm(const QString &str)
{
    // currently only "rsa-sha1" or "rsa-sha256"
    const QStringList lst = str.split(QLatin1Char('-'));
    if (lst.count() != 2) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "algorithm is invalid " << str;
        //Error
    } else {
        mSigningAlgorithm = lst.at(0);
        const QString hashStr = lst.at(1);
        if (hashStr == QLatin1String("sha1")) {
            mHashingAlgorithm = HashingAlgorithmType::Sha1;
        } else if (hashStr == QLatin1String("sha256")) {
            mHashingAlgorithm = HashingAlgorithmType::Sha256;
        } else {
            mHashingAlgorithm = HashingAlgorithmType::Unknown;
        }
    }
}

QString DKIMInfo::iDomain() const
{
    return mIDomain;
}

void DKIMInfo::setIDomain(const QString &iDomain)
{
    mIDomain = iDomain;
}

void DKIMInfo::parseCanonicalization(const QString &str)
{
    if (!str.isEmpty()) {
        const QStringList canonicalizations = str.split(QLatin1Char('/'));
        //qDebug() << " canonicalizations "<< canonicalizations;
        if (canonicalizations.count() >= 1) {
            if (canonicalizations.at(0) == QLatin1String("relaxed")) {
                mHeaderCanonization = DKIMInfo::Relaxed;
            } else if (canonicalizations.at(0) == QLatin1String("simple")) {
                mHeaderCanonization = DKIMInfo::Simple;
            } else {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "canonicalizations for header unknown " << canonicalizations.at(0);
                mHeaderCanonization = DKIMInfo::Unknown;
                return;
            }
            if (canonicalizations.count() == 1) {
                mBodyCanonization = DKIMInfo::Simple;
            } else if (canonicalizations.count() == 2) {
                if (canonicalizations.at(1) == QLatin1String("relaxed")) {
                    mBodyCanonization = DKIMInfo::Relaxed;
                } else if (canonicalizations.at(1) == QLatin1String("simple")) {
                    mBodyCanonization = DKIMInfo::Simple;
                } else {
                    qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "canonicalizations for body unknown " << canonicalizations.at(1);
                    mBodyCanonization = DKIMInfo::Unknown;
                    return;
                }
            } else {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Problem during parsing canonicalizations " << str;
                mHeaderCanonization = DKIMInfo::Unknown;
                mBodyCanonization = DKIMInfo::Unknown;
            }
        }
    }
}

QStringList DKIMInfo::copiedHeaderField() const
{
    return mCopiedHeaderField;
}

void DKIMInfo::setCopiedHeaderField(const QStringList &copiedHeaderField)
{
    mCopiedHeaderField = copiedHeaderField;
}

DKIMInfo::CanonicalizationType DKIMInfo::bodyCanonization() const
{
    return mBodyCanonization;
}

void DKIMInfo::setBodyCanonization(const CanonicalizationType &bodyCanonization)
{
    mBodyCanonization = bodyCanonization;
}

bool DKIMInfo::operator==(const DKIMInfo &other) const
{
    return mVersion == other.version()
           && mHashingAlgorithm == other.hashingAlgorithm()
           && mSigningAlgorithm == other.signingAlgorithm()
           && mDomain == other.domain()
           && mSelector == other.selector()
           && mBodyHash == other.bodyHash()
           && mSignatureTimeStamp == other.signatureTimeStamp()
           && mExpireTime == other.expireTime()
           && mQuery == other.query()
           && mSignature == other.signature()
           && mAgentOrUserIdentifier == other.agentOrUserIdentifier()
           && mBodyLengthCount == other.bodyLengthCount()
           && mListSignedHeader == other.listSignedHeader()
           && mHeaderCanonization == other.headerCanonization()
           && mBodyCanonization == other.bodyCanonization()
           && mIDomain == other.iDomain();
}

DKIMInfo::CanonicalizationType DKIMInfo::headerCanonization() const
{
    return mHeaderCanonization;
}

void DKIMInfo::setHeaderCanonization(const CanonicalizationType &headerCanonization)
{
    mHeaderCanonization = headerCanonization;
}

int DKIMInfo::version() const
{
    return mVersion;
}

void DKIMInfo::setVersion(int version)
{
    mVersion = version;
}

DKIMInfo::HashingAlgorithmType DKIMInfo::hashingAlgorithm() const
{
    return mHashingAlgorithm;
}

void DKIMInfo::setHashingAlgorithm(DKIMInfo::HashingAlgorithmType hashingAlgorithm)
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
    if (mBodyCanonization == DKIMInfo::Unknown || mHeaderCanonization == DKIMInfo::Unknown) {
        return false;
    }

    return !mSelector.isEmpty() && !mDomain.isEmpty() && !mBodyHash.isEmpty()
           && ((mHashingAlgorithm == HashingAlgorithmType::Sha1) || mHashingAlgorithm == HashingAlgorithmType::Sha256);
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

qint64 DKIMInfo::signatureTimeStamp() const
{
    return mSignatureTimeStamp;
}

void DKIMInfo::setSignatureTimeStamp(qint64 signatureTimeStamp)
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

qint64 DKIMInfo::expireTime() const
{
    return mExpireTime;
}

void DKIMInfo::setExpireTime(qint64 expireTime)
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

QString DKIMInfo::agentOrUserIdentifier() const
{
    return mAgentOrUserIdentifier;
}

void DKIMInfo::setAgentOrUserIdentifier(const QString &userAgent)
{
    mAgentOrUserIdentifier = userAgent;
}

int DKIMInfo::bodyLengthCount() const
{
    return mBodyLengthCount;
}

void DKIMInfo::setBodyLengthCount(int bodyLengthCount)
{
    mBodyLengthCount = bodyLengthCount;
}

QDebug operator <<(QDebug d, const DKIMInfo &t)
{
    d << "mVersion " << t.version();
    d << "mHashingAlgorithm " << t.hashingAlgorithm();
    d << "mSigningAlgorithm " << t.signingAlgorithm();
    d << "mDomain " << t.domain();
    d << "mSelector " << t.selector();
    d << "mBodyHash " << t.bodyHash();
    d << "mSignatureTimeStamp " << t.signatureTimeStamp();
    d << "mExpireTime " << t.expireTime();
    d << "mQuery " << t.query();
    d << "mSignature " << t.signature();
    d << "mAgentOrUserIdentifier " << t.agentOrUserIdentifier();
    d << "mBodyLengthCount " << t.bodyLengthCount();
    d << "mListSignedHeader " << t.listSignedHeader();
    d << "mHeaderCanonization " << t.headerCanonization();
    d << "mBodyCanonization " << t.bodyCanonization();
    d << "mIdomain " << t.iDomain();
    return d;
}
