/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

#ifndef DKIMCHECKFULLJOB_H
#define DKIMCHECKFULLJOB_H

#include <QObject>
#include "dkimauthenticationstatusinfo.h"
#include "dkimchecksignaturejob.h"
#include "messageviewer_private_export.h"
#include <MessageViewer/DKIMCheckPolicy>
#include <AkonadiCore/Item>
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMCheckFullJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckFullJob(QObject *parent = nullptr);
    ~DKIMCheckFullJob();

    void startCheckFullInfo(const KMime::Message::Ptr &message);
    void startCheckFullInfo(const Akonadi::Item &item);

    Q_REQUIRED_RESULT DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);

Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);

private:
    void slotCheckSignatureResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void slotCheckAuthenticationStatusResult(const MessageViewer::DKIMAuthenticationStatusInfo &info);
    void checkFullInfo(const Akonadi::Item &item);
    void checkSignature(const QVector<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> &lst = {});
    void checkDKim(const KMime::Message::Ptr &message);
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
#endif // DKIMCHECKFULLJOB_H
