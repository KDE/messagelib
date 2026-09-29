/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimchecksignaturejob.h"
#include "messagecore_private_export.h"
#include <QObject>
namespace MessageCore
{
class MESSAGECORE_EXPORT DKIMGenerateRuleJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMGenerateRuleJob(QObject *parent = nullptr);
    ~DKIMGenerateRuleJob() override;

    [[nodiscard]] bool canStart() const;
    [[nodiscard]] bool start();

    [[nodiscard]] DKIMCheckSignatureJob::CheckSignatureResult result() const;
    void setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);

private:
    MESSAGECORE_NO_EXPORT void verifyAndGenerateRule();
    DKIMCheckSignatureJob::CheckSignatureResult mResult;
};
}
