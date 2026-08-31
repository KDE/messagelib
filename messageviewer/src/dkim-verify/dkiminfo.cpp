/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkiminfo.h"

#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"

using namespace Qt::Literals::StringLiterals;
using namespace MessageViewer;

DKIMInfo::DKIMInfo() = default;

bool DKIMInfo::parseDKIM(const QString &header)
{
    if (header.isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Error: trying to parse empty header";
        return false;
    }
    QString newHeaders = header;
    newHeaders.replace("; "_L1, ";"_L1);
    const QStringList items = newHeaders.split(u';', Qt::SkipEmptyParts);
    bool foundCanonizations = false;
    for (int i = 0; i < items.count(); ++i) {
        const QString elem = items.at(i).trimmed();
        if (elem.startsWith("v="_L1)) {
            mVersion = QStringView(elem).right(elem.length() - 2).toInt();
            if (mVersion != 1) {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Version is not correct " << mVersion;
            }
        } else if (elem.startsWith("a="_L1)) {
            // Parse it as "algorithm.signature-algorithm.hash
            parseAlgorithm(elem.right(elem.length() - 2));
        } else if (elem.startsWith("t="_L1)) {
            mSignatureTimeStamp = elem.right(elem.length() - 2).toLong();
        } else if (elem.startsWith("c="_L1)) {
            // Parse header/body canonicalization (example c=relaxed/simple) only relaxed and simple.
            parseCanonicalization(elem.right(elem.length() - 2));
            foundCanonizations = true;
        } else if (elem.startsWith("bh="_L1)) {
            mBodyHash = elem.right(elem.length() - 3).remove(u' ');
        } else if (elem.startsWith("l="_L1)) {
            mBodyLengthCount = QStringView(elem).right(elem.length() - 2).toInt();
        } else if (elem.startsWith("i="_L1)) {
            mAgentOrUserIdentifier = elem.right(elem.length() - 2);
        } else if (elem.startsWith("q="_L1)) {
            mQuery = elem.right(elem.length() - 2);
            if (mQuery != "dns/txt"_L1) {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Query is not correct and not supported " << mQuery;
            }
        } else if (elem.startsWith("d="_L1)) {
            mDomain = elem.right(elem.length() - 2).trimmed();
        } else if (elem.startsWith("s="_L1)) {
            mSelector = elem.right(elem.length() - 2).trimmed();
        } else if (elem.startsWith("b="_L1)) {
            mSignature = elem.right(elem.length() - 2);
        } else if (elem.startsWith("h="_L1)) {
            const QString str = MessageViewer::DKIMUtil::cleanString(elem.right(elem.length() - 2));
            mListSignedHeader = str.split(u':');
        } else if (elem.startsWith("x="_L1)) {
            mExpireTime = elem.right(elem.length() - 2).toLong();
        } else if (elem.startsWith("z="_L1)) {
            mCopiedHeaderField = elem.right(elem.length() - 2).split(u':');
        } else {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Unknown element type" << elem << " : items : " << items;
        }
    }
    if (!foundCanonizations) { // Default
        mHeaderCanonization = Simple;
        mBodyCanonization = Simple;
    }
    if (mVersion == -1) {
        mVersion = 1;
    }
    if (mQuery.isEmpty()) {
        mQuery = u"dns/txt"_s;
    }
    if (mAgentOrUserIdentifier.isEmpty()) {
        mAgentOrUserIdentifier = u'@' + mDomain;
        mIDomain = mDomain;
    } else {
        const QStringList lst = mAgentOrUserIdentifier.split(u'@');
        if (lst.count() == 2) {
            mAgentOrUserIdentifier = u'@' + mDomain;
            mIDomain = lst.at(1);
        }
    }
    return true;
}

void DKIMInfo::parseAlgorithm(const QString &str)
{
    // currently only "rsa-sha1" or "rsa-sha256"
    const QStringList lst = str.split(u'-');
    if (lst.count() != 2) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "algorithm is invalid " << str;
        // Error
    } else {
        mSigningAlgorithm = lst.at(0);
        const QString hashStr = lst.at(1);
        if (hashStr == "sha1"_L1) {
            mHashingAlgorithm = HashingAlgorithmType::Sha1;
        } else if (hashStr == "sha256"_L1) {
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
        const QList<QStringView> canonicalization = QStringView(str).split(u'/');
        // qDebug() << " canonicalization "<< canonicalization;
        if (canonicalization.count() >= 1) {
            if (canonicalization.at(0) == "relaxed"_L1) {
                mHeaderCanonization = DKIMInfo::Relaxed;
            } else if (canonicalization.at(0) == "simple"_L1) {
                mHeaderCanonization = DKIMInfo::Simple;
            } else {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "canonicalization for header unknown " << canonicalization.at(0);
                mHeaderCanonization = DKIMInfo::Unknown;
                return;
            }
            if (canonicalization.count() == 1) {
                mBodyCanonization = DKIMInfo::Simple;
            } else if (canonicalization.count() == 2) {
                if (canonicalization.at(1) == "relaxed"_L1) {
                    mBodyCanonization = DKIMInfo::Relaxed;
                } else if (canonicalization.at(1) == "simple"_L1) {
                    mBodyCanonization = DKIMInfo::Simple;
                } else {
                    qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "canonicalization for body unknown " << canonicalization.at(1);
                    mBodyCanonization = DKIMInfo::Unknown;
                    return;
                }
            } else {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Problem during parsing canonicalization " << str;
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

void DKIMInfo::setBodyCanonization(CanonicalizationType bodyCanonization)
{
    mBodyCanonization = bodyCanonization;
}

bool DKIMInfo::operator==(const DKIMInfo &other) const
{
    return mVersion == other.version() && mHashingAlgorithm == other.hashingAlgorithm() && mSigningAlgorithm == other.signingAlgorithm()
        && mDomain == other.domain() && mSelector == other.selector() && mBodyHash == other.bodyHash() && mSignatureTimeStamp == other.signatureTimeStamp()
        && mExpireTime == other.expireTime() && mQuery == other.query() && mSignature == other.signature()
        && mAgentOrUserIdentifier == other.agentOrUserIdentifier() && mBodyLengthCount == other.bodyLengthCount()
        && mListSignedHeader == other.listSignedHeader() && mHeaderCanonization == other.headerCanonization() && mBodyCanonization == other.bodyCanonization()
        && mIDomain == other.iDomain();
}

DKIMInfo::CanonicalizationType DKIMInfo::headerCanonization() const
{
    return mHeaderCanonization;
}

void DKIMInfo::setHeaderCanonization(CanonicalizationType headerCanonization)
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

QDebug operator<<(QDebug d, const DKIMInfo &t)
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

#include "moc_dkiminfo.cpp"
