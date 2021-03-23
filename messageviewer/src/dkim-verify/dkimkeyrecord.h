/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

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
 * @brief The DKIMKeyRecord class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMKeyRecord
{
public:
    DKIMKeyRecord();
    Q_REQUIRED_RESULT bool parseKey(const QString &key);

    Q_REQUIRED_RESULT QString version() const;
    void setVersion(const QString &version);

    Q_REQUIRED_RESULT QString keyType() const;
    void setKeyType(const QString &keyType);

    Q_REQUIRED_RESULT QString note() const;
    void setNote(const QString &note);

    Q_REQUIRED_RESULT QString publicKey() const;
    void setPublicKey(const QString &publicKey);

    Q_REQUIRED_RESULT QString service() const;
    void setService(const QString &service);

    Q_REQUIRED_RESULT QStringList hashAlgorithm() const;
    void setHashAlgorithm(const QStringList &hashAlgorithm);

    Q_REQUIRED_RESULT QStringList flags() const;
    void setFlags(const QStringList &flags);

    Q_REQUIRED_RESULT bool operator==(const DKIMKeyRecord &other) const;

private:
    QString mVersion;
    QString mKeyType;
    QString mNote;
    QString mPublicKey;
    QString mService;
    QStringList mHashAlgorithm;
    QStringList mFlags;
};
}
Q_DECLARE_METATYPE(MessageViewer::DKIMKeyRecord)
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMKeyRecord &t);

