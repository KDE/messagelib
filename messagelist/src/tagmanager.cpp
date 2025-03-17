/*
    SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "tagmanager.h"
#include <Akonadi/Monitor>
using namespace MessageList::Core;
TagManager::TagManager(QObject *parent)
    : QObject{parent}
    , mMonitor(new Akonadi::Monitor(this))
{
}

TagManager::~TagManager() = default;

TagManager *TagManager::self()
{
    static TagManager s_self;
    return &s_self;
}

void TagManager::init()
{
    mMonitor->setObjectName(QLatin1StringView("MessageListTagMonitor"));
    mMonitor->setTypeMonitored(Akonadi::Monitor::Tags);
    connect(mMonitor, &Akonadi::Monitor::tagAdded, this, &TagManager::slotTagsChanged);
    connect(mMonitor, &Akonadi::Monitor::tagRemoved, this, &TagManager::slotTagsChanged);
    connect(mMonitor, &Akonadi::Monitor::tagChanged, this, &TagManager::slotTagsChanged);
}

void TagManager::slotTagsChanged()
{
    // TODO
}

#include "moc_tagmanager.cpp"
