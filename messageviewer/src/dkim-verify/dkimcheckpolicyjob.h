/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimchecksignaturejob.h"
#include "dmarcpolicyjob.h"
#include "messageviewer_export.h"
#include <QObject>
namespace MessageViewer
{
/**
 * @brief The DKIMCheckPolicyJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMCheckPolicyJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckPolicyJob(QObject *parent = nullptr);
    ~DKIMCheckPolicyJob() override;
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
    void dmarcPolicyResult(const MessageViewer::DMARCPolicyJob::DMARCResult &value, const QString &emailAddress);
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult mCheckResult;
    QString mEmailAddress;
    DKIMCheckPolicy mPolicy;
};
}

