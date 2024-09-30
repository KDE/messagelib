/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QHash>
#include <QWidget>
class QPushButton;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchLineCommandWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchLineCommandWidget(QWidget *parent = nullptr);
    ~SearchLineCommandWidget() override;

Q_SIGNALS:
    void insertCommand(const QString &commandStr);

private:
    MESSAGELIST_NO_EXPORT void fillWidgets();
    MESSAGELIST_NO_EXPORT QPushButton *createPushButton(bool needSpace, const QString &i18nStr, const QString &commandStr);
    struct ButtonInfo {
        bool needSpace = false;
        QString identifier;
        QString i18n;
    };

    QList<ButtonInfo> mButtonsList;
};
}
}
