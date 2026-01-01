/*
    SPDX-FileCopyrightText: 2025-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "tagmanager.h"

#include "messagelist_debug.h"
#include <Akonadi/Monitor>
#include <Akonadi/TagAttribute>
#include <Akonadi/TagFetchJob>
#include <Akonadi/TagFetchScope>
using namespace MessageList::Core;
using namespace Qt::Literals::StringLiterals;
TagManager::TagManager(QObject *parent)
    : QObject{parent}
    , mMonitor(new Akonadi::Monitor(this))
{
    init();
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
    slotTagsChanged();
}

void TagManager::slotTagsChanged()
{
    auto fetchJob = new Akonadi::TagFetchJob(this);
    fetchJob->fetchScope().fetchAttribute<Akonadi::TagAttribute>();
    connect(fetchJob, &Akonadi::TagFetchJob::result, this, &TagManager::slotTagsFetched);
}

QMap<QString, QString> TagManager::mapTag() const
{
    return mMapTag;
}

void TagManager::setMapTag(const QMap<QString, QString> &newMapTag)
{
    mMapTag = newMapTag;
}

void TagManager::slotTagsFetched(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGELIST_LOG) << "Failed to load tags " << job->errorString();
        return;
    }
    mMapTag.clear();
    auto fetchJob = static_cast<Akonadi::TagFetchJob *>(job);
    const auto tags{fetchJob->tags()};
    Q_EMIT tagsFetched(tags);

    for (const Akonadi::Tag &akonadiTag : tags) {
        const QString tagUrl = akonadiTag.url().url();
        mMapTag.insert(akonadiTag.name(), tagUrl);
    }
}

QString TagManager::tagFromName(const QString &name) const
{
    return mMapTag.value(name);
}

#include "moc_tagmanager.cpp"
