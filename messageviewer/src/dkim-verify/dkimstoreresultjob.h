/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMSTORERESULTJOB_H
#define DKIMSTORERESULTJOB_H

#include <QObject>
#include "messageviewer_private_export.h"
#include <MessageViewer/DKIMCheckSignatureJob>
#include <AkonadiCore/Item>
class KJob;
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMStoreResultJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMStoreResultJob(QObject *parent = nullptr);
    ~DKIMStoreResultJob();

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

#endif // DKIMSTORERESULTJOB_H
