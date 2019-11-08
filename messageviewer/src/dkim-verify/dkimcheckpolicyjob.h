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

#ifndef DKIMCHECKPOLICYJOB_H
#define DKIMCHECKPOLICYJOB_H

#include <QObject>
#include "dkimchecksignaturejob.h"
#include "dmarcpolicyjob.h"
#include "messageviewer_export.h"
namespace MessageViewer {
/**
 * @brief The DKIMCheckPolicyJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMCheckPolicyJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckPolicyJob(QObject *parent = nullptr);
    ~DKIMCheckPolicyJob();
    Q_REQUIRED_RESULT bool canStart() const;
    Q_REQUIRED_RESULT bool start();

    Q_REQUIRED_RESULT MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult checkResult() const;
    void setCheckResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);

    Q_REQUIRED_RESULT QString emailAddress() const;
    void setEmailAddress(const QString &emailAddress);

    Q_REQUIRED_RESULT DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);

Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);

private:
    void compareWithDefaultRules();
    void dmarcPolicyResult(const MessageViewer::DMARCPolicyJob::DMARCResult &value);
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult mCheckResult;
    QString mEmailAddress;
    DKIMCheckPolicy mPolicy;
};
}

#endif // DKIMCHECKPOLICYJOB_H
