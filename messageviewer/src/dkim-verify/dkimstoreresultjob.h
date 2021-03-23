/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <AkonadiCore/Item>
#include <MessageViewer/DKIMCheckSignatureJob>
#include <QObject>
class KJob;
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DKIMStoreResultJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMStoreResultJob(QObject *parent = nullptr);
    ~DKIMStoreResultJob() override;

    void start();
    Q_REQUIRED_RESULT bool canStart() const;

    void setResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void setItem(const Akonadi::Item &item);

private:
    void slotModifyItemDone(KJob *job);
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult mResult;
    Akonadi::Item mItem;
};
}

