/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_private_export.h"
#include <KMessageWidget>

namespace MessageViewer
{
/**
 * @brief The MDNWarningWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_TESTS_EXPORT MDNWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit MDNWarningWidget(QWidget *parent = nullptr);
    ~MDNWarningWidget() override;

    void setCanDeny(bool deny);

Q_SIGNALS:
    void ignoreMdn();
    void sendMdn();
    void sendDeny();

private:
    void slotSend();
    void slotIgnore();
    void slotSendDeny();
    QAction *const mIgnoreAction;
    QAction *const mSendAction;
    QAction *const mSendDenyAction;
};
}
