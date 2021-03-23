/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimchecksignaturejob.h"
#include "messageviewer_private_export.h"
#include <QObject>
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DKIMGenerateRuleJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMGenerateRuleJob(QObject *parent = nullptr);
    ~DKIMGenerateRuleJob() override;

    Q_REQUIRED_RESULT bool canStart() const;
    Q_REQUIRED_RESULT bool start();

    Q_REQUIRED_RESULT DKIMCheckSignatureJob::CheckSignatureResult result() const;
    void setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);

private:
    void verifyAndGenerateRule();
    DKIMCheckSignatureJob::CheckSignatureResult mResult;
};
}
