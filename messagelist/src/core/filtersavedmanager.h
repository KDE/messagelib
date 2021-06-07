/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

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
    };
    explicit FilterSavedManager(QObject *parent = nullptr);
    ~FilterSavedManager() override;

    void saveFilter(MessageList::Core::Filter *filter, const QString &filtername);
    void loadMenu(QMenu *menu);
    Q_REQUIRED_RESULT QVector<FilterInfo> filterInfos() const;

    static FilterSavedManager *self();

    void removeFilter(const QString &identifier);
    Q_REQUIRED_RESULT Filter *loadFilter(const QString &identifier);
Q_SIGNALS:
    void activateFilter(const QString &identifier);
};
}
}
Q_DECLARE_TYPEINFO(MessageList::Core::FilterSavedManager::FilterInfo, Q_MOVABLE_TYPE);
