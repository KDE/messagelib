/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

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
    [[nodiscard]] bool parseKey(const QString &key);

    [[nodiscard]] QString version() const;
    void setVersion(const QString &version);

    [[nodiscard]] QString keyType() const;
    void setKeyType(const QString &keyType);

    [[nodiscard]] QString note() const;
    void setNote(const QString &note);

    [[nodiscard]] QString publicKey() const;
    void setPublicKey(const QString &publicKey);

    [[nodiscard]] QString service() const;
    void setService(const QString &service);

    [[nodiscard]] QStringList hashAlgorithm() const;
    void setHashAlgorithm(const QStringList &hashAlgorithm);

    [[nodiscard]] QStringList flags() const;
    void setFlags(const QStringList &flags);

    [[nodiscard]] bool operator==(const DKIMKeyRecord &other) const;

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
