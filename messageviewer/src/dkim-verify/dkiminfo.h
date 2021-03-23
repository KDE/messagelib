/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
namespace MessageViewer
{
/**
 * @brief The DKIMInfo class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMInfo
{
    Q_GADGET
public:
    DKIMInfo();

    enum CanonicalizationType {
        Unknown,
        Simple,
        Relaxed,
    };
    enum class HashingAlgorithmType {
        Any,
        Sha1,
        Sha256,
        Unknown,
    };
    Q_ENUM(HashingAlgorithmType)

    Q_REQUIRED_RESULT bool parseDKIM(const QString &header);

    Q_REQUIRED_RESULT int version() const;
    void setVersion(int version);

    Q_REQUIRED_RESULT HashingAlgorithmType hashingAlgorithm() const;
    void setHashingAlgorithm(DKIMInfo::HashingAlgorithmType type);

    Q_REQUIRED_RESULT QString domain() const;
    void setDomain(const QString &domain);

    Q_REQUIRED_RESULT QString selector() const;
    void setSelector(const QString &selector);

    Q_REQUIRED_RESULT QString bodyHash() const;
    void setBodyHash(const QString &bodyHash);

    Q_REQUIRED_RESULT bool isValid() const;

    Q_REQUIRED_RESULT QStringList listSignedHeader() const;
    void setListSignedHeader(const QStringList &listSignedHeader);

    Q_REQUIRED_RESULT QString signingAlgorithm() const;
    void setSigningAlgorithm(const QString &signingAlgorithm);

    Q_REQUIRED_RESULT qint64 signatureTimeStamp() const;
    void setSignatureTimeStamp(qint64 signatureTimeStamp);

    Q_REQUIRED_RESULT QString query() const;
    void setQuery(const QString &query);

    Q_REQUIRED_RESULT qint64 expireTime() const;
    void setExpireTime(qint64 expireTime);

    Q_REQUIRED_RESULT QString signature() const;
    void setSignature(const QString &signature);

    Q_REQUIRED_RESULT QString agentOrUserIdentifier() const;
    void setAgentOrUserIdentifier(const QString &agentOrUserIdentifier);

    Q_REQUIRED_RESULT int bodyLengthCount() const;
    void setBodyLengthCount(int bodyLengthCount);

    Q_REQUIRED_RESULT CanonicalizationType headerCanonization() const;
    void setHeaderCanonization(CanonicalizationType headerCanonization);

    Q_REQUIRED_RESULT CanonicalizationType bodyCanonization() const;
    void setBodyCanonization(CanonicalizationType bodyCanonization);

    Q_REQUIRED_RESULT bool operator==(const DKIMInfo &other) const;

    Q_REQUIRED_RESULT QStringList copiedHeaderField() const;
    void setCopiedHeaderField(const QStringList &copiedHeaderField);

    Q_REQUIRED_RESULT QString iDomain() const;
    void setIDomain(const QString &iDomain);

private:
    void parseCanonicalization(const QString &str);
    void parseAlgorithm(const QString &str);
    HashingAlgorithmType mHashingAlgorithm = HashingAlgorithmType::Any;
    QString mSigningAlgorithm;
    QString mDomain;
    QString mSelector;
    QString mBodyHash;
    QString mQuery;
    QString mSignature;
    QString mAgentOrUserIdentifier;
    QString mIDomain;
    QStringList mListSignedHeader;
    QStringList mCopiedHeaderField; // Default is empty
    qint64 mSignatureTimeStamp = -1;
    qint64 mExpireTime = -1;
    int mVersion = -1;
    int mBodyLengthCount = -1;
    CanonicalizationType mHeaderCanonization = Unknown;
    CanonicalizationType mBodyCanonization = Unknown;
};
}
Q_DECLARE_METATYPE(MessageViewer::DKIMInfo)
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMInfo &t);
