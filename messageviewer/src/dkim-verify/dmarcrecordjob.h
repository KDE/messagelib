/*
  SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

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

    Q_REQUIRED_RESULT bool start();

    Q_REQUIRED_RESULT bool canStart() const;

    Q_REQUIRED_RESULT QString domainName() const;
    void setDomainName(const QString &domainName);

Q_SIGNALS:
    void success(const QList<QByteArray> &lst, const QString &domainName);
    void error(const QString &err, const QString &domainName);

private:
    void resolvDnsDone();
    Q_REQUIRED_RESULT QString resolvDnsValue() const;
    QString mDomainName;
    QDnsLookup *mDnsLookup = nullptr;
};
}

