/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_export.h"
#include <QWidget>
class QButtonGroup;
class QHBoxLayout;
namespace MessageList
{
namespace Core
{
// TODO use private export symbol
class MESSAGELIST_EXPORT SearchStatusButtons : public QWidget
{
    Q_OBJECT
public:
    explicit SearchStatusButtons(QWidget *parent = nullptr);
    ~SearchStatusButtons() override;

private:
    MESSAGELIST_NO_EXPORT void createAction();
    MESSAGELIST_NO_EXPORT void createFilterAction(const QIcon &icon, const QString &text, int value);
    QButtonGroup *const mButtonGroup;
    QHBoxLayout *const mMainLayout;
};
}
}
