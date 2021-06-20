/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QWidget>
class QLineEdit;
class KIconButton;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT FilterNameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterNameWidget(QWidget *parent = nullptr);
    ~FilterNameWidget() override;

    Q_REQUIRED_RESULT QString filterName() const;

    void setExistingFilterNames(const QStringList &lst);
    Q_REQUIRED_RESULT QString iconName() const;
Q_SIGNALS:
    void updateOkButton(bool enabled);

private:
    QStringList mFilterListNames;
    QLineEdit *const mName;
    KIconButton *const mIconButton;
};
}
}
