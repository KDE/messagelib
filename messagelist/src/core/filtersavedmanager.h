/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>
class QMenu;
namespace MessageList
{
namespace Core
{
class Filter;
class FilterSavedManager : public QObject
{
    Q_OBJECT
public:
    struct FilterInfo {
        QString filterName;
        QString identifier;
        QString iconName;
    };
    ~FilterSavedManager() override;

    void saveFilter(MessageList::Core::Filter *filter, const QString &filtername, const QString &iconName);
    void loadMenu(QMenu *menu);
    [[nodiscard]] QList<FilterInfo> filterInfos() const;

    static FilterSavedManager *self();

    void removeFilter(const QString &identifier);
    void updateFilter(const QString &identifier, const QString &newName, const QString &iconName = {});
    [[nodiscard]] Filter *loadFilter(const QString &identifier);
    [[nodiscard]] QStringList existingFilterNames() const;

Q_SIGNALS:
    void activateFilter(const QString &identifier);

private:
    explicit FilterSavedManager(QObject *parent = nullptr);
};
}
}
Q_DECLARE_TYPEINFO(MessageList::Core::FilterSavedManager::FilterInfo, Q_RELOCATABLE_TYPE);
