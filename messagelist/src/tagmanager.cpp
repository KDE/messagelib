/*
    SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "tagmanager.h"
#include "messagelist_debug.h"
#include <Akonadi/Monitor>
#include <Akonadi/TagAttribute>
#include <Akonadi/TagFetchJob>
#include <Akonadi/TagFetchScope>
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
    auto fetchJob = new Akonadi::TagFetchJob(this);
    fetchJob->fetchScope().fetchAttribute<Akonadi::TagAttribute>();
    connect(fetchJob, &Akonadi::TagFetchJob::result, this, &TagManager::slotTagsFetched);

    // TODO
}

void TagManager::slotTagsFetched(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGELIST_LOG) << "Failed to load tags " << job->errorString();
        return;
    }
    auto fetchJob = static_cast<Akonadi::TagFetchJob *>(job);
    /*

        KConfigGroup conf(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListView"));
        const QString tagSelected = conf.readEntry(QStringLiteral("TagSelected"));
        if (tagSelected.isEmpty()) {
            setCurrentStatusFilterItem();
            return;
        }
        const QStringList tagSelectedLst = tagSelected.split(QLatin1Char(','));

        addMessageTagItem(QIcon::fromTheme(QStringLiteral("mail-flag")).pixmap(16, 16),
                          i18nc("Item in list of Akonadi tags, to show all e-mails", "All"),
                          QVariant());

        QStringList tagFound;
        const auto tags{fetchJob->tags()};
        for (const Akonadi::Tag &akonadiTag : tags) {
            const QString tagUrl = akonadiTag.url().url();
            if (tagSelectedLst.contains(tagUrl)) {
                tagFound.append(akonadiTag.url().url());
                const QString label = akonadiTag.name();
                const QString id = tagUrl;
                const auto attr = akonadiTag.attribute<Akonadi::TagAttribute>();
                const QString iconName = attr ? attr->iconName() : QStringLiteral("mail-tagged");
                addMessageTagItem(QIcon::fromTheme(iconName).pixmap(16, 16), label, QVariant(id));
            }
        }
        conf.writeEntry(QStringLiteral("TagSelected"), tagFound);
        conf.sync();

        setCurrentStatusFilterItem();
        */
}

#include "moc_tagmanager.cpp"
