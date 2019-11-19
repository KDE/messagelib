/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#ifndef DKIMMANAGER_H
#define DKIMMANAGER_H

#include <QObject>
#include "messageviewer_export.h"
#include "dkimchecksignaturejob.h"
#include <KMime/Message>
#include <AkonadiCore/Item>
namespace MessageViewer {
class DKIMAuthenticationStatusInfo;
/**
 * @brief The DKIMManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManager : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManager(QObject *parent = nullptr);
    ~DKIMManager();
    static DKIMManager *self();

    void checkDKim(const KMime::Message::Ptr &message);
    void checkDKim(const Akonadi::Item &item);

    void clearInfoWidget();

    Q_REQUIRED_RESULT DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);
Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void clearInfo();
private:
    void checkSignature(const Akonadi::Item &item);
    void storeResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void storeKey(const QString &key, const QString &domain, const QString &selector);
    void slotCheckSignatureResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void slotCheckAuthenticationStatusResult(const MessageViewer::DKIMAuthenticationStatusInfo &info, const Akonadi::Item &item);
    DKIMCheckPolicy mCheckPolicy;
};
}

#endif // DKIMMANAGER_H
