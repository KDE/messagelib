/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

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

class MESSAGELIST_TESTS_EXPORT ConfigureFiltersListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ConfigureFiltersListWidget(QWidget *parent = nullptr);
    ~ConfigureFiltersListWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;
};

class MESSAGELIST_TESTS_EXPORT ConfigureFiltersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigureFiltersWidget(QWidget *parent = nullptr);
    ~ConfigureFiltersWidget() override;

private:
    MESSAGELIST_NO_EXPORT void updateFilterInfo(const QString &identifier, const QString &newName, const QString &newIconName = {});
    MESSAGELIST_NO_EXPORT void slotCustomContextMenuRequested(const QPoint &pos);
    MESSAGELIST_NO_EXPORT void removeFilterInfo(const QString &identifier);
    MESSAGELIST_NO_EXPORT void slotConfigureFilter(QListWidgetItem *item);
    MESSAGELIST_NO_EXPORT void init();
    ConfigureFiltersListWidget *const mListFiltersWidget;
};
}
}
