/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mdnwarningwidgetjob.h"
#include <Akonadi/Item>
#include <Akonadi/ItemModifyJob>
#ifdef ENABLE_JOB
#include <MessageComposer/MDNStateAttribute>
#endif
using namespace MessageViewer;
MDNWarningWidgetJob::MDNWarningWidgetJob(QObject *parent)
    : QObject{parent}
{
}

MDNWarningWidgetJob::~MDNWarningWidgetJob()
{
}

void MDNWarningWidgetJob::start()
{
    if (!canStart()) {
        deleteLater();
        return;
    }
#if 0
    const Akonadi::Collection collection = item.parentCollection();
    if (collection.isValid()
        && (CommonKernel->folderIsSentMailFolder(collection) || CommonKernel->folderIsTrash(collection) || CommonKernel->folderIsDraftOrOutbox(collection)
            || CommonKernel->folderIsTemplates(collection))) {
        deleteLater();
        return;
    }

    const KMime::Message::Ptr message = MessageComposer::Util::message(item);
    if (!message) {
        deleteLater();
        return;
    }

    const QPair<bool, KMime::MDN::SendingMode> mdnSend = MessageComposer::MDNAdviceHelper::instance()->checkAndSetMDNInfo(item, KMime::MDN::Displayed);
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

#endif
    // TODO
}

const Akonadi::Item &MDNWarningWidgetJob::item() const
{
    return mItem;
}

void MDNWarningWidgetJob::setItem(const Akonadi::Item &newItem)
{
    mItem = newItem;
}

bool MDNWarningWidgetJob::canStart() const
{
    return mItem.isValid();
}

void MDNWarningWidgetJob::modifyItem()
{
#ifdef ENABLE_JOB
    auto mdnStateAttr = new MessageComposer::MDNStateAttribute(MessageComposer::MDNStateAttribute::MDNStateUnknown);
    // create a minimal version of item with just the attribute we want to change
    Akonadi::Item i(mItem.id());
    i.setRevision(mItem.revision());
    i.setMimeType(mItem.mimeType());
    i.addAttribute(mdnStateAttr);
    auto modify = new Akonadi::ItemModifyJob(i);
    modify->setIgnorePayload(true);
    modify->disableRevisionCheck();
#endif
}
