/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>
namespace MessageList
{
namespace Core
{
class Filter;
class FilterSavedManager : public QObject
{
    Q_OBJECT
public:
    explicit FilterSavedManager(QObject *parent = nullptr);
    ~FilterSavedManager() override;

    void saveFilter(MessageList::Core::Filter *filter, const QString &filtername);
};
}
}
