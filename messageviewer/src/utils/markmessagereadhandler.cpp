/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileContributor: Tobias Koenig <tokoe@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "markmessagereadhandler.h"

#include "settings/messageviewersettings.h"

#include <Akonadi/ItemModifyJob>
#include <Akonadi/MessageFlags>
#include <Akonadi/Session>

#include <QTimer>

using namespace MessageViewer;
Q_GLOBAL_STATIC(Akonadi::Item::List, sListItem)

class MarkMessageReadHandler::MarkMessageReadHandlerPrivate
{
public:
    explicit MarkMessageReadHandlerPrivate(MarkMessageReadHandler *qq)
        : q(qq)
    {
    }

    void handleMessages();
    void cancelPendingMessage();

    MarkMessageReadHandler *const q;
    Akonadi::Item mItemQueue;
    QTimer mTimer;
};

void MarkMessageReadHandler::MarkMessageReadHandlerPrivate::handleMessages()
{
    Akonadi::Item item = mItemQueue;

    // mark as read
    item.setFlag(Akonadi::MessageFlags::Seen);

    auto modifyJob = new Akonadi::ItemModifyJob(item, q->session());
    modifyJob->disableRevisionCheck();
    modifyJob->setIgnorePayload(true);
    sListItem->removeAll(item);
}

// Give up on the message that is still waiting for its timer. It must not be left behind in
// sListItem once its timer has been cancelled, otherwise the contains() check in setItem()
// would keep skipping that message and it would never be marked as read.
void MarkMessageReadHandler::MarkMessageReadHandlerPrivate::cancelPendingMessage()
{
    mTimer.stop();
    if (mItemQueue.isValid()) {
        sListItem->removeAll(mItemQueue);
        mItemQueue = Akonadi::Item();
    }
}

MarkMessageReadHandler::MarkMessageReadHandler(QObject *parent)
    : QObject(parent)
    , d(new MarkMessageReadHandlerPrivate(this))
{
    d->mTimer.setSingleShot(true);
    connect(&d->mTimer, &QTimer::timeout, this, [this]() {
        d->handleMessages();
    });
}

MarkMessageReadHandler::~MarkMessageReadHandler()
{
    d->cancelPendingMessage();
}

void MarkMessageReadHandler::setItem(const Akonadi::Item &item)
{
    if (MessageViewer::MessageViewerSettings::self()->delayedMarkAsRead()) {
        if (sListItem->contains(item) || d->mItemQueue == item || item.hasFlag(Akonadi::MessageFlags::Queued)) {
            return;
        }

        d->cancelPendingMessage();

        if (item.hasFlag(Akonadi::MessageFlags::Seen)) {
            return;
        }

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
