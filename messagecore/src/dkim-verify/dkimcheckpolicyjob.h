/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimchecksignaturejob.h"
#include "dmarcpolicyjob.h"
#include "messagecore_export.h"
#include <QObject>
namespace MessageCore
{
/*!
 * \class MessageCore::DKIMCheckPolicyJob
 * \inmodule MessageCore
 * \inheaderfile MessageCore/DKIMCheckPolicyJob
 * \brief The DKIMCheckPolicyJob class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGECORE_EXPORT DKIMCheckPolicyJob : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    explicit DKIMCheckPolicyJob(QObject *parent = nullptr);
    /*!
     */
    ~DKIMCheckPolicyJob() override;
    /*!
     */
    [[nodiscard]] bool canStart() const;
    /*!
     */
    [[nodiscard]] bool start();

    /*!
     */
    [[nodiscard]] MessageCore::DKIMCheckSignatureJob::CheckSignatureResult checkResult() const;
    /*!
     */
    void setCheckResult(const MessageCore::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);

    /*!
     */
    [[nodiscard]] QString emailAddress() const;
    /*!
     */
    void setEmailAddress(const QString &emailAddress);

    /*!
     */
    [[nodiscard]] DKIMCheckPolicy policy() const;
    /*!
     */
    void setPolicy(const DKIMCheckPolicy &policy);

Q_SIGNALS:
    /*!
     */
    void result(const MessageCore::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);

private:
    MESSAGECORE_NO_EXPORT void compareWithDefaultRules();
    MESSAGECORE_NO_EXPORT void dmarcPolicyResult(const MessageCore::DMARCPolicyJob::DMARCResult &value, const QString &emailAddress);
    MessageCore::DKIMCheckSignatureJob::CheckSignatureResult mCheckResult;
    QString mEmailAddress;
    DKIMCheckPolicy mPolicy;
};
}
