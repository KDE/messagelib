/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "dmarcpolicyjob.h"
#include "dmarcrecordjob.h"
#include "dmarcinfo.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace MessageViewer;

DMARCPolicyJob::DMARCPolicyJob(QObject *parent)
    : QObject(parent)
{
}

DMARCPolicyJob::~DMARCPolicyJob()
{
}

bool DMARCPolicyJob::canStart() const
{
    return !mEmailAddress.isEmpty();
}

bool DMARCPolicyJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Impossible to start DMARCPolicyJob";
        deleteLater();
        //TODO emit result
        //Q_EMIT ... //TODO
        return false;
    }

    const QString emailDomainStr = emailDomain();
    DMARCRecordJob *job = new DMARCRecordJob(this);
    //TODO ???
    connect(job, &MessageViewer::DMARCRecordJob::success, this, &DMARCPolicyJob::slotCheckDomain);

    connect(job, &MessageViewer::DMARCRecordJob::error, this, [](const QString &err, const QString &domainName) {
        qDebug() << "error: " << err << " domain " << domainName;
    });

    job->setDomainName(emailDomainStr);
    if (!job->start()) {
        //TODO emit result
        deleteLater();
        return false;
    }
    return true;
}


void DMARCPolicyJob::slotCheckDomain(const QList<QByteArray> &lst, const QString &domainName)
{
    QByteArray ba;
    if (lst.count() != 1) {
        for (const QByteArray &b : lst) {
            ba += b;
        }
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "DMARCPolicyJob Key result has more that 1 element" << lst;
    } else {
        ba = lst.at(0);
    }

    DMARCInfo info;
    if (info.parseDMARC(QString::fromLocal8Bit(ba))) {
        //TODO check if valid
        if (info.version() != QLatin1String("DMARC1")) {
            //Invalid
        } else if (info.policy().isEmpty()) {
            //Invalid
        } else if (info.percentage() > 100 || info.percentage() < 0) {
            //Invalid
        } else {
            //Valid send info.
        }
    } else {
        //TODO
    }


    qDebug() << "domainName: " << domainName << " lst " << lst;
    //Parse result
}

QString DMARCPolicyJob::emailDomain() const
{
    //TODO
    return {};
}

QString DMARCPolicyJob::emailAddress() const
{
    return mEmailAddress;
}

void DMARCPolicyJob::setEmailAddress(const QString &emailAddress)
{
    mEmailAddress = emailAddress;
}
