/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <KMessageWidget>

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

Q_SIGNALS:
    void sendResponse(MessageViewer::MDNWarningWidget::ResponseType type);

private:
    void slotSend();
    void slotIgnore();
    void slotSendDeny();
    QAction *const mIgnoreAction;
    QAction *const mSendAction;
    QAction *const mSendDenyAction;
};
}
