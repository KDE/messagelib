/*
  SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <QObject>
class QDnsLookup;
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DMARCRecordJob : public QObject
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
    MESSAGEVIEWER_NO_EXPORT void resolvDnsDone();
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString resolvDnsValue() const;
    QString mDomainName;
    QDnsLookup *mDnsLookup = nullptr;
};
}
