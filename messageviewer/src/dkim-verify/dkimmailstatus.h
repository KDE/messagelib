/// SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>
/// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
/// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "dkimchecksignaturejob.h"
#include <Akonadi/Item>

namespace MessageViewer
{
/**
 * @brief The DKIMMailStatus class is responsible to handle the result of the
 * DKIMCheckSignatureJob for the currently selected item set with currentItemId.
 * @author Carl Schwan <carl@carlschwan.eu>
 */
class MESSAGEVIEWER_EXPORT DKIMMailStatus : public QObject
{
    Q_OBJECT

    /// This property holds the status text of the currentItemId dkim status.
    Q_PROPERTY(QString text READ text NOTIFY textChanged)

    /// This property holds the additional information for the currentItemId dkim status.
    Q_PROPERTY(QString tooltip READ tooltip NOTIFY tooltipChanged)

    /// This property holds the status code of the currentItemId.
    Q_PROPERTY(MessageViewer::DKIMCheckSignatureJob::DKIMStatus status READ status NOTIFY statusChanged)

    /// This property holds the error code of the currentItemId.
    Q_PROPERTY(MessageViewer::DKIMCheckSignatureJob::DKIMError error READ error NOTIFY errorChanged)

    /// This property holds the currentItemId for which we want the dkim status.
    Q_PROPERTY(Akonadi::Item::Id currentItemId READ currentItemId WRITE setCurrentItemId NOTIFY currentItemIdChanged)

public:
    explicit DKIMMailStatus(QObject *parent = nullptr);
    ~DKIMMailStatus() override = default;

    QString text() const;
    QString tooltip() const;
    DKIMCheckSignatureJob::DKIMStatus status() const;
    DKIMCheckSignatureJob::DKIMError error() const;

    Q_REQUIRED_RESULT MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult result() const;
    void setResult(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id);

    Q_REQUIRED_RESULT Akonadi::Item::Id currentItemId() const;
    void setCurrentItemId(Akonadi::Item::Id currentItemId);

    Q_INVOKABLE void clear();

    void checkDkim(Akonadi::Item messageItem);

Q_SIGNALS:
    void statusChanged();
    void errorChanged();
    void textChanged();
    void currentItemIdChanged();
    void tooltipChanged();

private:
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult mResult;
    Akonadi::Item::Id mCurrentItemId = -1;
};
}
