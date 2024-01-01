/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimauthenticationstatusinfo.h"
#include "dkimchecksignaturejob.h"
#include "messageviewer_private_export.h"
#include <Akonadi/Item>
#include <MessageViewer/DKIMCheckPolicy>
#include <QObject>
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DKIMCheckFullJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckFullJob(QObject *parent = nullptr);
    ~DKIMCheckFullJob() override;

    void startCheckFullInfo(const KMime::Message::Ptr &message);
    void startCheckFullInfo(const Akonadi::Item &item);

    [[nodiscard]] DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);

Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);

private:
    void slotCheckSignatureResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void slotCheckAuthenticationStatusResult(const MessageViewer::DKIMAuthenticationStatusInfo &info);
    void checkSignature(const QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> &lst = {});
    void storeKey(const QString &key, const QString &domain, const QString &selector);
    void storeInKeyManager(const QString &key, const QString &domain, const QString &selector, bool verify);
    void storeResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void generateRule(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void checkAuthenticationResults();
    DKIMCheckPolicy mCheckPolicy;
    KMime::Message::Ptr mMessage;
    Akonadi::Item mAkonadiItem;
    DKIMHeaderParser mHeaderParser;
};
}
