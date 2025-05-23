/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QFrame>
#include <QHash>
class QPushButton;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchLineCommandButtonsWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SearchLineCommandButtonsWidget(QWidget *parent = nullptr);
    ~SearchLineCommandButtonsWidget() override;

Q_SIGNALS:
    void insertCommand(const QString &commandStr);

private:
    struct ButtonInfo {
        QString identifier;
        QString i18n;
    };
    MESSAGELIST_NO_EXPORT QList<ButtonInfo> fillCommandLineText() const;
    [[nodiscard]] MESSAGELIST_NO_EXPORT QPushButton *createPushButton(const QString &i18nStr, const QString &commandStr);
};
}
}
