/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimchecksignaturejob.h"
#include "messageviewer_export.h"
#include <Akonadi/Item>
#include <KMime/Message>
#include <QObject>

namespace MessageViewer
{
/**
 * \class MessageViewer::DKIMManager
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/DKIMManager
 * \brief The DKIMManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManager : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    explicit DKIMManager(QObject *parent = nullptr);
    /*!
     */
    ~DKIMManager() override;
    /*!
     */
    static DKIMManager *self();

    /*!
     */
    void checkDKim(const std::shared_ptr<KMime::Message> &message);
    /*!
     */
    void checkDKim(const Akonadi::Item &item);

    /*!
     */
    void clearInfoWidget();

    /*!
     */
    [[nodiscard]] DKIMCheckPolicy policy() const;
    /*!
     */
    void setPolicy(const DKIMCheckPolicy &policy);
    /*!
     */
    void recheckDKim(const Akonadi::Item &item);
Q_SIGNALS:
    /*!
     */
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);
    /*!
     */
    void clearInfo();

private:
    MESSAGEVIEWER_NO_EXPORT void checkFullInfo(const Akonadi::Item &item);
    DKIMCheckPolicy mCheckPolicy;
};
}
