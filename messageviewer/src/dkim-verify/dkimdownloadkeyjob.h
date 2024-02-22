/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
class QDnsLookup;
namespace MessageViewer
{
/**
 * @brief The DKIMDownloadKeyJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMDownloadKeyJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMDownloadKeyJob(QObject *parent = nullptr);
    ~DKIMDownloadKeyJob() override;

    [[nodiscard]] bool start();

    [[nodiscard]] bool canStart() const;

    [[nodiscard]] QString domainName() const;
    void setDomainName(const QString &domainName);

    [[nodiscard]] QString selectorName() const;
    void setSelectorName(const QString &selectorName);

    [[nodiscard]] QString resolvDnsValue() const;

    [[nodiscard]] QDnsLookup *dnsLookup() const;

Q_SIGNALS:
    void error(const QString &error);
    void success(const QList<QByteArray> &, const QString &domain, const QString &selector);

private:
    MESSAGEVIEWER_NO_EXPORT void resolvDnsDone();
    QString mDomainName;
    QString mSelectorName;
    QDnsLookup *mDnsLookup = nullptr;
};
}
