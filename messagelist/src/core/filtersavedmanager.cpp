/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filtersavedmanager.h"
#include "filter.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QMenu>
using namespace MessageList::Core;
FilterSavedManager::FilterSavedManager(QObject *parent)
    : QObject(parent)
{
}

FilterSavedManager::~FilterSavedManager()
{
}

FilterSavedManager *FilterSavedManager::self()
{
    static FilterSavedManager s_self;
    return &s_self;
}

void FilterSavedManager::saveFilter(MessageList::Core::Filter *filter, const QString &filtername)
{
    filter->save(KSharedConfig::openConfig(), filtername);
}

void FilterSavedManager::loadMenu(QMenu *menu)
{
    KConfigGroup grp(KSharedConfig::openConfig(), "General");
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), QStringLiteral("Filter_%1").arg(i));
        auto act = menu->addAction(newGroup.readEntry(QStringLiteral("name")));
        const QString identifier = newGroup.readEntry(QStringLiteral("identifier"));
        connect(act, &QAction::triggered, this, [this, identifier]() {
            Q_EMIT activateFilter(identifier);
        });
    }
}

QVector<FilterSavedManager::FilterInfo> FilterSavedManager::filterInfos() const
{
    // TODO
    return {};
}

void FilterSavedManager::removeFilter(const QString &identifier)
{
    // TODO
}
