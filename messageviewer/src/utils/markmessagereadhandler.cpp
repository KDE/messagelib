/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileContributor: Tobias Koenig <tokoe@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "markmessagereadhandler.h"

#include "settings/messageviewersettings.h"

#include <Akonadi/KMime/MessageFlags>
#include <AkonadiCore/Session>
#include <AkonadiCore/itemmodifyjob.h>

#include <QTimer>

using namespace MessageViewer;
Q_GLOBAL_STATIC(Akonadi::Item::List, sListItem)

class Q_DECL_HIDDEN MarkMessageReadHandler::Private
{
public:
    Private(MarkMessageReadHandler *qq)
        : q(qq)
    {
    }

    void handleMessages();

    MarkMessageReadHandler *const q;
    Akonadi::Item mItemQueue;
    QTimer mTimer;
};

void MarkMessageReadHandler::Private::handleMessages()
{
    Akonadi::Item item = mItemQueue;

    // mark as read
    item.setFlag(Akonadi::MessageFlags::Seen);

    auto modifyJob = new Akonadi::ItemModifyJob(item, q->session());
    modifyJob->disableRevisionCheck();
    modifyJob->setIgnorePayload(true);
    sListItem->removeAll(item);
}

MarkMessageReadHandler::MarkMessageReadHandler(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->mTimer.setSingleShot(true);
    connect(&d->mTimer, &QTimer::timeout, this, [this]() {
        d->handleMessages();
    });
}

MarkMessageReadHandler::~MarkMessageReadHandler()
{
    if (d->mTimer.isActive()) {
        d->mTimer.stop();
    }
    delete d;
}

void MarkMessageReadHandler::setItem(const Akonadi::Item &item)
{
    if (MessageViewer::MessageViewerSettings::self()->delayedMarkAsRead()) {
        if (sListItem->contains(item) || d->mItemQueue == item || item.hasFlag(Akonadi::MessageFlags::Queued)) {
            return;
        }
        if (d->mTimer.isActive()) {
            d->mTimer.stop();
        }
        if (item.hasFlag(Akonadi::MessageFlags::Seen)) {
            return;
        }

        sListItem->removeAll(d->mItemQueue);
        d->mItemQueue = item;
        sListItem->append(item);

        const int delayedMarkTime = MessageViewer::MessageViewerSettings::self()->delayedMarkTime();
        if (delayedMarkTime != 0) {
            d->mTimer.start(delayedMarkTime * 1000);
        } else {
            d->handleMessages();
        }
    }
}

#include "moc_markmessagereadhandler.cpp"
