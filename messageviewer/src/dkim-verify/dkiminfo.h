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

#ifndef DKIMINFO_H
#define DKIMINFO_H

#include "messageviewer_private_export.h"
#include <QString>
#include <QStringList>
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMInfo
{
public:
    DKIMInfo();

    Q_REQUIRED_RESULT bool parseDKIM(const QString &header);
    Q_REQUIRED_RESULT QString version() const;
    void setVersion(const QString &version);

    Q_REQUIRED_RESULT QString hashingAlgorithm() const;
    void setHashingAlgorithm(const QString &hashingAlgorithm);

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

    Q_REQUIRED_RESULT QString signatureTimeStamp() const;
    void setSignatureTimeStamp(const QString &signatureTimeStamp);

    Q_REQUIRED_RESULT QString query() const;
    void setQuery(const QString &query);

    Q_REQUIRED_RESULT QString expireTime() const;
    void setExpireTime(const QString &expireTime);

    Q_REQUIRED_RESULT QString signature() const;
    void setSignature(const QString &signature);

    Q_REQUIRED_RESULT QString userAgent() const;
    void setUserAgent(const QString &userAgent);

    Q_REQUIRED_RESULT QString bodyLenghtCount() const;
    void setBodyLenghtCount(const QString &bodyLenghtCount);

private:
    QString mVersion;
    void parseCanonicalization(const QString &str);
    QString mHashingAlgorithm;
    QString mSigningAlgorithm;
    QString mDomain;
    QString mSelector;
    QString mBodyHash;
    QString mSignatureTimeStamp;
    QString mExpireTime;
    QString mQuery;
    QString mSignature;
    QString mUserAgent;
    QString mBodyLenghtCount;
    QStringList mListSignedHeader;    
};
}

#endif // DKIMINFO_H
