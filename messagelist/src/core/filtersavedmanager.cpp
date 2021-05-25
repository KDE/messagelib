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
