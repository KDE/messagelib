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

#ifndef DMARCRECORDJOB_H
#define DMARCRECORDJOB_H

#include <QObject>
#include "messageviewer_private_export.h"
class QDnsLookup;
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DMARCRecordJob : public QObject
{
    Q_OBJECT
public:
    explicit DMARCRecordJob(QObject *parent = nullptr);
    ~DMARCRecordJob();

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

#endif // DMARCRECORDJOB_H
