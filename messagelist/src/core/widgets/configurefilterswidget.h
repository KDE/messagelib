/*
  SPDX-FileCopyrightText: 2021-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QListWidget>
#include <QWidget>
namespace MessageList
{
namespace Core
{
class FilterListWidgetItem : public QListWidgetItem
{
public:
    explicit FilterListWidgetItem(QListWidget *parent = nullptr);
    ~FilterListWidgetItem() override;

    [[nodiscard]] const QString &identifier() const;
    void setIdentifier(const QString &newIdentifier);

    [[nodiscard]] const QString &iconName() const;
    void setIconName(const QString &newIconName);

private:
    QString mIdentifier;
    QString mIconName;
};

class MESSAGELIST_TESTS_EXPORT ConfigureFiltersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigureFiltersWidget(QWidget *parent = nullptr);
    ~ConfigureFiltersWidget() override;

private:
    void updateFilterInfo(const QString &identifier, const QString &newName, const QString &newIconName = {});
    void slotCustomContextMenuRequested(const QPoint &pos);
    void removeFilterInfo(const QString &identifier);
    void slotConfigureFilter(QListWidgetItem *item);
    void init();
    QListWidget *const mListFiltersWidget;
};
}
}
