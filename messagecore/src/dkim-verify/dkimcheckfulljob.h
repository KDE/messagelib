/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimauthenticationstatusinfo.h"
#include "dkimchecksignaturejob.h"
#include "messagecore_private_export.h"
#include <Akonadi/Item>
#include <MessageCore/DKIMCheckPolicy>
#include <QObject>
namespace MessageCore
{
class MESSAGECORE_EXPORT DKIMCheckFullJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckFullJob(QObject *parent = nullptr);
    ~DKIMCheckFullJob() override;

    void startCheckFullInfo(const std::shared_ptr<KMime::Message> &message);
    void startCheckFullInfo(const Akonadi::Item &item);

    [[nodiscard]] DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);

Q_SIGNALS:
    void result(const MessageCore::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);

private:
    MESSAGECORE_NO_EXPORT void slotCheckSignatureResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    MESSAGECORE_NO_EXPORT void slotCheckAuthenticationStatusResult(const MessageCore::DKIMAuthenticationStatusInfo &info);
    MESSAGECORE_NO_EXPORT void checkSignature(const QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> &lst = {});
    MESSAGECORE_NO_EXPORT void storeKey(const QString &key, const QString &domain, const QString &selector);
    MESSAGECORE_NO_EXPORT void storeInKeyManager(const QString &key, const QString &domain, const QString &selector, bool verify);
    MESSAGECORE_NO_EXPORT void storeResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    MESSAGECORE_NO_EXPORT void generateRule(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    MESSAGECORE_NO_EXPORT void checkAuthenticationResults();
    DKIMCheckPolicy mCheckPolicy;
    std::shared_ptr<KMime::Message> mMessage;
    Akonadi::Item mAkonadiItem;
    DKIMHeaderParser mHeaderParser;
};
}
