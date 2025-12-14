/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filtersavedmanager.h"

#include "filter.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QMenu>
#include <QRegularExpression>
using namespace Qt::Literals::StringLiterals;
using namespace MessageList::Core;
FilterSavedManager::FilterSavedManager(QObject *parent)
    : QObject(parent)
{
}

FilterSavedManager::~FilterSavedManager() = default;

FilterSavedManager *FilterSavedManager::self()
{
    static FilterSavedManager s_self;
    return &s_self;
}

QStringList FilterSavedManager::existingFilterNames() const
{
    QStringList lst;
    KConfigGroup grp(KSharedConfig::openConfig(), u"General"_s);
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    lst.reserve(numberFilter);
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), u"Filter_%1"_s.arg(i));
        lst << newGroup.readEntry(u"name"_s);
    }
    return lst;
}

void FilterSavedManager::saveFilter(MessageList::Core::Filter *filter, const QString &filtername, const QString &iconName)
{
    filter->save(KSharedConfig::openConfig(), filtername, iconName);
}

void FilterSavedManager::loadMenu(QMenu *menu)
{
    KConfigGroup grp(KSharedConfig::openConfig(), u"General"_s);
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), u"Filter_%1"_s.arg(i));
        const QString iconName = newGroup.readEntry(u"iconName"_s);
        auto act = menu->addAction(QIcon::fromTheme(iconName), newGroup.readEntry(u"name"_s));
        const QString identifier = newGroup.readEntry(u"identifier"_s);
        connect(act, &QAction::triggered, this, [this, identifier]() {
            Q_EMIT activateFilter(identifier);
        });
    }
}

QList<FilterSavedManager::FilterInfo> FilterSavedManager::filterInfos() const
{
    KConfigGroup grp(KSharedConfig::openConfig(), u"General"_s);
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    QList<FilterSavedManager::FilterInfo> lst;
    lst.reserve(numberFilter);
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), u"Filter_%1"_s.arg(i));

        FilterSavedManager::FilterInfo info;
        info.filterName = newGroup.readEntry(u"name"_s);
        info.identifier = newGroup.readEntry(u"identifier"_s);
        info.iconName = newGroup.readEntry(u"iconName"_s);
        lst << info;
    }
    return lst;
}

Filter *FilterSavedManager::loadFilter(const QString &identifier)
{
    const QStringList list = KSharedConfig::openConfig()->groupList().filter(QRegularExpression(u"Filter_\\d+"_s));
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
    KConfigGroup grp(KSharedConfig::openConfig(), u"General"_s);
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    QList<Filter *> lst;
    lst.reserve(numberFilter);
    for (int i = 0; i < numberFilter; ++i) {
        Filter *f = Filter::load(KSharedConfig::openConfig(), i);
        lst << f;
    }

    const QStringList list = KSharedConfig::openConfig()->groupList().filter(QRegularExpression(u"Filter_\\d+"_s));
    for (const QString &group : list) {
        KSharedConfig::openConfig()->deleteGroup(group);
    }

    int numberOfFilter = 0;
    for (Filter *f : std::as_const(lst)) {
        if ((f->identifier() != identifier) && !f->identifier().isEmpty()) {
            f->save(KSharedConfig::openConfig(), f->filterName(), f->iconName(), numberOfFilter);
            numberOfFilter++;
        }
    }
    grp.writeEntry("NumberFilter", numberOfFilter);
    grp.sync();
    qDeleteAll(lst);
    lst.clear();
}

void FilterSavedManager::updateFilter(const QString &identifier, const QString &newName, const QString &iconName)
{
    KConfigGroup grp(KSharedConfig::openConfig(), u"General"_s);
    const int numberFilter = grp.readEntry("NumberFilter").toInt();
    for (int i = 0; i < numberFilter; ++i) {
        KConfigGroup newGroup(KSharedConfig::openConfig(), u"Filter_%1"_s.arg(i));
        if (newGroup.readEntry("identifier") == identifier) {
            newGroup.writeEntry("name", newName);
            if (!iconName.isEmpty()) {
                newGroup.writeEntry("iconName", iconName);
            }
            newGroup.sync();
            break;
        }
    }
}

#include "moc_filtersavedmanager.cpp"
