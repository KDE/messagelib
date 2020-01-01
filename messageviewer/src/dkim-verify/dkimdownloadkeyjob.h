/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

#ifndef DKIMDOWNLOADKEYJOB_H
#define DKIMDOWNLOADKEYJOB_H

#include <QObject>
#include "messageviewer_export.h"
class QDnsLookup;
namespace MessageViewer {
/**
 * @brief The DKIMDownloadKeyJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMDownloadKeyJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMDownloadKeyJob(QObject *parent = nullptr);
    ~DKIMDownloadKeyJob();

    Q_REQUIRED_RESULT bool start();

    Q_REQUIRED_RESULT bool canStart() const;

    Q_REQUIRED_RESULT QString domainName() const;
    void setDomainName(const QString &domainName);

    Q_REQUIRED_RESULT QString selectorName() const;
    void setSelectorName(const QString &selectorName);

    Q_REQUIRED_RESULT QString resolvDnsValue() const;

    QDnsLookup *dnsLookup() const;

Q_SIGNALS:
    void error(const QString &error);
    void success(const QList<QByteArray> &, const QString &domain, const QString &selector);

private:
    void resolvDnsDone();
    QString mDomainName;
    QString mSelectorName;
    QDnsLookup *mDnsLookup = nullptr;
};
}

#endif // DKIMDOWNLOADKEYJOB_H
