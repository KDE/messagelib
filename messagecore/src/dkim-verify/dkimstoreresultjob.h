/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_private_export.h"
#include <Akonadi/Item>
#include <MessageCore/DKIMCheckSignatureJob>
#include <QObject>
class KJob;
namespace MessageCore
{
class MESSAGECORE_EXPORT DKIMStoreResultJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMStoreResultJob(QObject *parent = nullptr);
    ~DKIMStoreResultJob() override;

    void start();
    [[nodiscard]] bool canStart() const;

    void setResult(const MessageCore::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void setItem(const Akonadi::Item &item);

private:
    MESSAGECORE_NO_EXPORT void slotModifyItemDone(KJob *job);
    MessageCore::DKIMCheckSignatureJob::CheckSignatureResult mResult;
    Akonadi::Item mItem;
};
}
