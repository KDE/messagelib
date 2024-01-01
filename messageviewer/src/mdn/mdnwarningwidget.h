/*
  SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <KMessageWidget>
#include <KMime/MDN>

namespace MessageViewer
{
/**
 * @brief The MDNWarningWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MDNWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    enum ResponseType {
        Ignore = 0,
        Send = 1,
        SendDeny = 2,
    };
    explicit MDNWarningWidget(QWidget *parent = nullptr);
    ~MDNWarningWidget() override;

    void setCanDeny(bool deny);

    void setInformation(const QString &str);

    [[nodiscard]] KMime::MDN::SendingMode sendingMode() const;
    void setSendingMode(KMime::MDN::SendingMode newSendingMode);

Q_SIGNALS:
    void sendResponse(MessageViewer::MDNWarningWidget::ResponseType type, KMime::MDN::SendingMode sendingMode);

private:
    MESSAGEVIEWER_NO_EXPORT void slotSend();
    MESSAGEVIEWER_NO_EXPORT void slotIgnore();
    MESSAGEVIEWER_NO_EXPORT void slotSendDeny();
    QAction *const mIgnoreAction;
    QAction *const mSendAction;
    QAction *const mSendDenyAction;
    KMime::MDN::SendingMode mSendingMode = KMime::MDN::SentAutomatically;
};
}
