/*
  SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_private_export.h"
#include <QObject>
class QDnsLookup;
namespace MessageCore
{
class MESSAGECORE_EXPORT DMARCRecordJob : public QObject
{
    Q_OBJECT
public:
    explicit DMARCRecordJob(QObject *parent = nullptr);
    ~DMARCRecordJob() override;

    [[nodiscard]] bool start();

    [[nodiscard]] bool canStart() const;

    [[nodiscard]] QString domainName() const;
    void setDomainName(const QString &domainName);

Q_SIGNALS:
    void success(const QList<QByteArray> &lst, const QString &domainName);
    void error(const QString &err, const QString &domainName);

private:
    MESSAGECORE_NO_EXPORT void resolvDnsDone();
    [[nodiscard]] MESSAGECORE_NO_EXPORT QString resolvDnsValue() const;
    QString mDomainName;
    QDnsLookup *mDnsLookup = nullptr;
};
}
