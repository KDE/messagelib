/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimchecksignaturejob.h"
#include "messageviewer_export.h"
#include <AkonadiCore/Item>
#include <KMime/Message>
#include <QObject>

namespace MessageViewer
{
/**
 * @brief The DKIMManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManager : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManager(QObject *parent = nullptr);
    ~DKIMManager() override;
    static DKIMManager *self();

    void checkDKim(const KMime::Message::Ptr &message);
    void checkDKim(const Akonadi::Item &item);

    void clearInfoWidget();

    Q_REQUIRED_RESULT DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);
    void recheckDKim(const Akonadi::Item &item);
Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);
    void clearInfo();

private:
    void checkFullInfo(const Akonadi::Item &item);
    DKIMCheckPolicy mCheckPolicy;
};
}

