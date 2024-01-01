/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] bool parseDKIM(const QString &header);

    [[nodiscard]] int version() const;
    void setVersion(int version);

    [[nodiscard]] HashingAlgorithmType hashingAlgorithm() const;
    void setHashingAlgorithm(DKIMInfo::HashingAlgorithmType type);

    [[nodiscard]] QString domain() const;
    void setDomain(const QString &domain);

    [[nodiscard]] QString selector() const;
    void setSelector(const QString &selector);

    [[nodiscard]] QString bodyHash() const;
    void setBodyHash(const QString &bodyHash);

    [[nodiscard]] bool isValid() const;

    [[nodiscard]] QStringList listSignedHeader() const;
    void setListSignedHeader(const QStringList &listSignedHeader);

    [[nodiscard]] QString signingAlgorithm() const;
    void setSigningAlgorithm(const QString &signingAlgorithm);

    [[nodiscard]] qint64 signatureTimeStamp() const;
    void setSignatureTimeStamp(qint64 signatureTimeStamp);

    [[nodiscard]] QString query() const;
    void setQuery(const QString &query);

    [[nodiscard]] qint64 expireTime() const;
    void setExpireTime(qint64 expireTime);

    [[nodiscard]] QString signature() const;
    void setSignature(const QString &signature);

    [[nodiscard]] QString agentOrUserIdentifier() const;
    void setAgentOrUserIdentifier(const QString &agentOrUserIdentifier);

    [[nodiscard]] int bodyLengthCount() const;
    void setBodyLengthCount(int bodyLengthCount);

    [[nodiscard]] CanonicalizationType headerCanonization() const;
    void setHeaderCanonization(CanonicalizationType headerCanonization);

    [[nodiscard]] CanonicalizationType bodyCanonization() const;
    void setBodyCanonization(CanonicalizationType bodyCanonization);

    [[nodiscard]] bool operator==(const DKIMInfo &other) const;

    [[nodiscard]] QStringList copiedHeaderField() const;
    void setCopiedHeaderField(const QStringList &copiedHeaderField);

    [[nodiscard]] QString iDomain() const;
    void setIDomain(const QString &iDomain);

private:
    MESSAGEVIEWER_NO_EXPORT void parseCanonicalization(const QString &str);
    MESSAGEVIEWER_NO_EXPORT void parseAlgorithm(const QString &str);
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
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMInfo &t);
