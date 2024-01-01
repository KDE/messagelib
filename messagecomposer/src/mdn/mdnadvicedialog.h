/*
  SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>
  SPDX-FileCopyrightText: 2009 Michael Leupold <lemma@confuego.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <MessageComposer/MessageFactoryNG>

#include <QDialog>

namespace MessageComposer
{
class MESSAGECOMPOSER_EXPORT MDNAdviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MDNAdviceDialog(const QString &text, bool canDeny, QWidget *parent = nullptr);
    ~MDNAdviceDialog() override;

    [[nodiscard]] MessageComposer::MDNAdvice result() const;

private:
    MESSAGECOMPOSER_NO_EXPORT void slotUser1Clicked();
    MESSAGECOMPOSER_NO_EXPORT void slotUser2Clicked();
    MESSAGECOMPOSER_NO_EXPORT void slotYesClicked();
    MessageComposer::MDNAdvice m_result = MessageComposer::MDNIgnore;
};
}
