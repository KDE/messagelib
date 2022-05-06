/*
  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
  SPDX-FileContributor: Tobias Koenig <tokoe@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "sendmdnhandler.h"

#include "kernel/mailkernel.h"
#include "mailcommon_debug.h"
#include "mdnadvicehelper.h"
#include "util/mailutil.h"
#include <MessageComposer/MessageFactoryNG>
#include <MessageComposer/MessageSender>
#include <MessageComposer/Util>
#include <MessageViewer/MessageViewerSettings>

#include <Akonadi/Collection>
#include <Akonadi/Item>
#include <Akonadi/MessageFlags>

#include <QQueue>
#include <QTimer>

using namespace MailCommon;

class Q_DECL_HIDDEN SendMdnHandler::SendMdnHandlerPrivate
{
public:
    SendMdnHandlerPrivate(SendMdnHandler *qq, IKernel *kernel)
        : q(qq)
        , mKernel(kernel)
    {
    }

    void handleMessages();

    SendMdnHandler *const q;
    IKernel *const mKernel;
    QQueue<Akonadi::Item> mItemQueue;
    QTimer mTimer;
};

void SendMdnHandler::SendMdnHandlerPrivate::handleMessages()
{
    while (!mItemQueue.isEmpty()) {
        Akonadi::Item item = mItemQueue.dequeue();

#if 0
        // should we send an MDN?
        if (MessageViewer::MessageViewerSettings::notSendWhenEncrypted()
            && message()->encryptionState() != KMMsgNotEncrypted
            && message()->encryptionState() != KMMsgEncryptionStateUnknown) {
            return;
        }
#else
        qCDebug(MAILCOMMON_LOG) << "AKONADI PORT: Disabled code in  " << Q_FUNC_INFO;
#endif

        const Akonadi::Collection collection = item.parentCollection();
        if (collection.isValid()
            && (CommonKernel->folderIsSentMailFolder(collection) || CommonKernel->folderIsTrash(collection) || CommonKernel->folderIsDraftOrOutbox(collection)
                || CommonKernel->folderIsTemplates(collection))) {
            continue;
        }

        const KMime::Message::Ptr message = MessageComposer::Util::message(item);
        if (!message) {
            continue;
        }

        const QPair<bool, KMime::MDN::SendingMode> mdnSend = MDNAdviceHelper::instance()->checkAndSetMDNInfo(item, KMime::MDN::Displayed);
        if (mdnSend.first) {
            const int quote = MessageViewer::MessageViewerSettings::self()->quoteMessage();

            MessageComposer::MessageFactoryNG factory(message, Akonadi::Item().id());
            factory.setIdentityManager(mKernel->identityManager());
            factory.setFolderIdentity(MailCommon::Util::folderIdentity(item));

            const KMime::Message::Ptr mdn = factory.createMDN(KMime::MDN::ManualAction, KMime::MDN::Displayed, mdnSend.second, quote);
            if (mdn) {
                if (!mKernel->msgSender()->send(mdn)) {
                    qCDebug(MAILCOMMON_LOG) << "Sending failed.";
                }
            }
        }
    }
}

SendMdnHandler::SendMdnHandler(IKernel *kernel, QObject *parent)
    : QObject(parent)
    , d(new SendMdnHandlerPrivate(this, kernel))
{
    d->mTimer.setSingleShot(true);
    connect(&d->mTimer, &QTimer::timeout, this, [this]() {
        d->handleMessages();
    });
}

SendMdnHandler::~SendMdnHandler() = default;

void SendMdnHandler::setItem(const Akonadi::Item &item)
{
    if (item.hasFlag(Akonadi::MessageFlags::Seen)) {
        return;
    }

    d->mTimer.stop();

    d->mItemQueue.enqueue(item);

    if (MessageViewer::MessageViewerSettings::self()->delayedMarkAsRead() && MessageViewer::MessageViewerSettings::self()->delayedMarkTime() != 0) {
        d->mTimer.start(MessageViewer::MessageViewerSettings::self()->delayedMarkTime() * 1000);
        return;
    }

    d->handleMessages();
}

#include "moc_sendmdnhandler.cpp"
