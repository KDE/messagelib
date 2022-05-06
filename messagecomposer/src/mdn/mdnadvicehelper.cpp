/*
  SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mdnadvicehelper.h"
#include "filter/mdnadvicedialog.h"
#include "mailcommon_debug.h"
#include <Akonadi/ItemModifyJob>
#include <KCursorSaver>
#include <KLazyLocalizedString>
#include <KLocalizedString>
#include <MessageComposer/Util>
#include <MessageViewer/MessageViewerSettings>
#include <QPointer>
using MessageComposer::MessageFactoryNG;
using namespace MailCommon;

static const struct {
    const char *dontAskAgainID;
    bool canDeny;
    const KLazyLocalizedString text;
} mdnMessageBoxes[] = {
    {"mdnNormalAsk",
     true,
     kli18n("This message contains a request to return a notification "
            "about your reception of the message.\n"
            "You can either ignore the request or let the mail program "
            "send a \"denied\" or normal response.")},
    {"mdnUnknownOption",
     false,
     kli18n("This message contains a request to send a notification "
            "about your reception of the message.\n"
            "It contains a processing instruction that is marked as "
            "\"required\", but which is unknown to the mail program.\n"
            "You can either ignore the request or let the mail program "
            "send a \"failed\" response.")},
    {"mdnMultipleAddressesInReceiptTo",
     true,
     kli18n("This message contains a request to send a notification "
            "about your reception of the message,\n"
            "but it is requested to send the notification to more "
            "than one address.\n"
            "You can either ignore the request or let the mail program "
            "send a \"denied\" or normal response.")},
    {"mdnReturnPathEmpty",
     true,
     kli18n("This message contains a request to send a notification "
            "about your reception of the message,\n"
            "but there is no return-path set.\n"
            "You can either ignore the request or let the mail program "
            "send a \"denied\" or normal response.")},
    {"mdnReturnPathNotInReceiptTo",
     true,
     kli18n("This message contains a request to send a notification "
            "about your reception of the message,\n"
            "but the return-path address differs from the address "
            "the notification was requested to be sent to.\n"
            "You can either ignore the request or let the mail program "
            "send a \"denied\" or normal response.")},
};

static const int numMdnMessageBoxes = sizeof mdnMessageBoxes / sizeof *mdnMessageBoxes;

MDNAdviceHelper *MDNAdviceHelper::s_instance = nullptr;
MessageComposer::MDNAdvice MDNAdviceHelper::questionIgnoreSend(const QString &text, bool canDeny)
{
    MessageComposer::MDNAdvice rc = MessageComposer::MDNIgnore;
    QPointer<MDNAdviceDialog> dlg(new MDNAdviceDialog(text, canDeny));
    dlg->exec();
    if (dlg) {
        rc = dlg->result();
    }
    delete dlg;
    return rc;
}

QPair<bool, KMime::MDN::SendingMode> MDNAdviceHelper::checkAndSetMDNInfo(const Akonadi::Item &item, KMime::MDN::DispositionType d, bool forceSend)
{
    KMime::Message::Ptr msg = MessageComposer::Util::message(item);

    // RFC 2298: At most one MDN may be issued on behalf of each
    // particular recipient by their user agent.  That is, once an MDN
    // has been issued on behalf of a recipient, no further MDNs may be
    // issued on behalf of that recipient, even if another disposition
    // is performed on the message.
    if (item.hasAttribute<MailCommon::MDNStateAttribute>()
        && item.attribute<MailCommon::MDNStateAttribute>()->mdnState() != MailCommon::MDNStateAttribute::MDNStateUnknown) {
        // if already dealt with, don't do it again.
        return QPair<bool, KMime::MDN::SendingMode>(false, KMime::MDN::SentAutomatically);
    }
    auto *mdnStateAttr = new MailCommon::MDNStateAttribute(MailCommon::MDNStateAttribute::MDNStateUnknown);

    KMime::MDN::SendingMode s = KMime::MDN::SentAutomatically; // set to manual if asked user
    bool doSend = false;
    // default:
    int mode = MessageViewer::MessageViewerSettings::self()->defaultPolicy();
    if (forceSend) { // We must send it
        mode = 3;
    } else {
        if (!mode || (mode < 0) || (mode > 3)) {
            // early out for ignore:
            mdnStateAttr->setMDNState(MailCommon::MDNStateAttribute::MDNIgnore);
            s = KMime::MDN::SentManually;
        } else {
            if (MessageFactoryNG::MDNMDNUnknownOption(msg)) {
                mode = requestAdviceOnMDN("mdnUnknownOption");
                s = KMime::MDN::SentManually;
                // TODO set type to Failed as well
                //      and clear modifiers
            }

            if (MessageFactoryNG::MDNConfirmMultipleRecipients(msg)) {
                mode = requestAdviceOnMDN("mdnMultipleAddressesInReceiptTo");
                s = KMime::MDN::SentManually;
            }

            if (MessageFactoryNG::MDNReturnPathEmpty(msg)) {
                mode = requestAdviceOnMDN("mdnReturnPathEmpty");
                s = KMime::MDN::SentManually;
            }

            if (MessageFactoryNG::MDNReturnPathNotInRecieptTo(msg)) {
                mode = requestAdviceOnMDN("mdnReturnPathNotInReceiptTo");
                s = KMime::MDN::SentManually;
            }

            if (MessageFactoryNG::MDNRequested(msg)) {
                if (s != KMime::MDN::SentManually) {
                    // don't ask again if user has already been asked. use the users' decision
                    mode = requestAdviceOnMDN("mdnNormalAsk");
                    s = KMime::MDN::SentManually; // asked user
                }
            } else { // if message doesn't have a disposition header, never send anything.
                mode = 0;
            }
        }
    }

    // RFC 2298: An MDN MUST NOT be generated in response to an MDN.
    if (MessageComposer::Util::findTypeInMessage(msg.data(), "message", "disposition-notification")) {
        mdnStateAttr->setMDNState(MailCommon::MDNStateAttribute::MDNIgnore);
    } else if (mode == 0) { // ignore
        doSend = false;
        mdnStateAttr->setMDNState(MailCommon::MDNStateAttribute::MDNIgnore);
    } else if (mode == 2) { // denied
        doSend = true;
        mdnStateAttr->setMDNState(MailCommon::MDNStateAttribute::MDNDenied);
    } else if (mode == 3) { // the user wants to send. let's make sure we can, according to the RFC.
        doSend = true;
        mdnStateAttr->setMDNState(dispositionToSentState(d));
    }

    // create a minimal version of item with just the attribute we want to change
    Akonadi::Item i(item.id());
    i.setRevision(item.revision());
    i.setMimeType(item.mimeType());
    i.addAttribute(mdnStateAttr);
    auto modify = new Akonadi::ItemModifyJob(i);
    modify->setIgnorePayload(true);
    modify->disableRevisionCheck();
    return QPair<bool, KMime::MDN::SendingMode>(doSend, s);
}

MailCommon::MDNStateAttribute::MDNSentState MDNAdviceHelper::dispositionToSentState(KMime::MDN::DispositionType d)
{
    switch (d) {
    case KMime::MDN::Displayed:
        return MailCommon::MDNStateAttribute::MDNDisplayed;
    case KMime::MDN::Deleted:
        return MailCommon::MDNStateAttribute::MDNDeleted;
    case KMime::MDN::Dispatched:
        return MailCommon::MDNStateAttribute::MDNDispatched;
    case KMime::MDN::Processed:
        return MailCommon::MDNStateAttribute::MDNProcessed;
    case KMime::MDN::Denied:
        return MailCommon::MDNStateAttribute::MDNDenied;
    case KMime::MDN::Failed:
        return MailCommon::MDNStateAttribute::MDNFailed;
    default:
        return MailCommon::MDNStateAttribute::MDNStateUnknown;
    }
}

int MDNAdviceHelper::requestAdviceOnMDN(const char *what)
{
    for (int i = 0; i < numMdnMessageBoxes; ++i) {
        if (!qstrcmp(what, mdnMessageBoxes[i].dontAskAgainID)) {
            KCursorSaver saver(Qt::ArrowCursor);
            const MessageComposer::MDNAdvice answer = questionIgnoreSend(mdnMessageBoxes[i].text.toString(), mdnMessageBoxes[i].canDeny);
            switch (answer) {
            case MessageComposer::MDNSend:
                return 3;

            case MessageComposer::MDNSendDenied:
                return 2;

            // don't use 1, as that's used for 'default ask" in checkMDNHeaders
            default:
            case MessageComposer::MDNIgnore:
                return 0;
            }
        }
    }
    qCWarning(MAILCOMMON_LOG) << "didn't find data for message box \"" << what << "\"";
    return MessageComposer::MDNIgnore;
}
