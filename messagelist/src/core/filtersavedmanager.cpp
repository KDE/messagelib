/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filtersavedmanager.h"
#include "filter.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QMenu>
#include <QRegularExpression>
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

QStringList FilterSavedManager::existingFilterNames() const
{
    QStringList lst;
    KConfigGroup grp(KSharedConfig::openConfig(), "General");
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    lst.reserve(numberFilter);
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), QStringLiteral("Filter_%1").arg(i));
        lst << newGroup.readEntry(QStringLiteral("name"));
    }
    return lst;
}

void FilterSavedManager::saveFilter(MessageList::Core::Filter *filter, const QString &filtername, const QString &iconName)
{
    filter->save(KSharedConfig::openConfig(), filtername, iconName);
}

void FilterSavedManager::loadMenu(QMenu *menu)
{
    KConfigGroup grp(KSharedConfig::openConfig(), "General");
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), QStringLiteral("Filter_%1").arg(i));
        const QString iconName = newGroup.readEntry(QStringLiteral("iconName"));
        auto act = menu->addAction(QIcon::fromTheme(iconName), newGroup.readEntry(QStringLiteral("name")));
        const QString identifier = newGroup.readEntry(QStringLiteral("identifier"));
        connect(act, &QAction::triggered, this, [this, identifier]() {
            Q_EMIT activateFilter(identifier);
        });
    }
}

QVector<FilterSavedManager::FilterInfo> FilterSavedManager::filterInfos() const
{
    KConfigGroup grp(KSharedConfig::openConfig(), "General");
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    QVector<FilterSavedManager::FilterInfo> lst;
    lst.reserve(numberFilter);
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), QStringLiteral("Filter_%1").arg(i));

        FilterSavedManager::FilterInfo info;
        info.filterName = newGroup.readEntry(QStringLiteral("name"));
        info.identifier = newGroup.readEntry(QStringLiteral("identifier"));
        info.iconName = newGroup.readEntry(QStringLiteral("iconName"));
        lst << info;
    }
    return lst;
}

Filter *FilterSavedManager::loadFilter(const QString &identifier)
{
    const QStringList list = KSharedConfig::openConfig()->groupList().filter(QRegularExpression(QStringLiteral("Filter_\\d+")));
    for (const QString &group : list) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), group);
        if (newGroup.readEntry("identifier") == identifier) {
            Filter *f = Filter::loadFromConfigGroup(newGroup);
            return f;
        }
    }
    return {};
}

void FilterSavedManager::removeFilter(const QString &identifier)
{
    KConfigGroup grp(KSharedConfig::openConfig(), "General");
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    QVector<Filter *> lst;
    lst.reserve(numberFilter);
    for (int i = 0; i < numberFilter; ++i) {
        Filter *f = Filter::load(KSharedConfig::openConfig(), i);
        lst << f;
    }

    const QStringList list = KSharedConfig::openConfig()->groupList().filter(QRegularExpression(QStringLiteral("Filter_\\d+")));
    for (const QString &group : list) {
        KSharedConfig::openConfig()->deleteGroup(group);
    }

    int numberOfFilter = 0;
    for (Filter *f : std::as_const(lst)) {
        if ((f->identifier() != identifier) && !f->identifier().isEmpty()) {
            f->save(KSharedConfig::openConfig(), f->filterName(), f->iconName());
            numberOfFilter++;
        }
    }
    grp.writeEntry("NumberFilter", numberOfFilter);
    grp.sync();
    qDeleteAll(lst);
    lst.clear();
}

void FilterSavedManager::renameFilter(const QString &identifier, const QString &newName)
{
    KConfigGroup grp(KSharedConfig::openConfig(), "General");
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), QStringLiteral("Filter_%1").arg(i));
        if (newGroup.readEntry("identifier") == identifier) {
            newGroup.writeEntry("name", newName);
            newGroup.sync();
            break;
        }
    }
}
