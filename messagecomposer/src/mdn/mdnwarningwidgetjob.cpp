/*
  SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mdnwarningwidgetjob.h"
#include "messagecomposer_debug.h"
#include <MessageComposer/MDNAdviceHelper>
#include <MessageComposer/MessageFactoryNG>
#include <MessageComposer/Util>
#include <MessageViewer/MessageViewerSettings>
using namespace MessageComposer;

MDNWarningWidgetJob::MDNWarningWidgetJob(QObject *parent)
    : QObject{parent}
{
}

MDNWarningWidgetJob::~MDNWarningWidgetJob() = default;

bool MDNWarningWidgetJob::canStart() const
{
    return mItem.isValid();
}

bool MDNWarningWidgetJob::start()
{
    QPair<QString, bool> mdnInfo;
    KMime::MDN::SendingMode s = KMime::MDN::SentAutomatically; // set to manual if asked user
    bool result = false;
    if (canStart()) {
        KMime::Message::Ptr msg = MessageComposer::Util::message(mItem);
        int mode = MessageViewer::MessageViewerSettings::self()->defaultPolicy();
        if (!mode || (mode < 0) || (mode > 3)) {
            // Nothing
        } else {
            if (MessageComposer::MessageFactoryNG::MDNMDNUnknownOption(msg)) {
                mdnInfo = MessageComposer::MDNAdviceHelper::instance()->mdnMessageText("mdnUnknownOption");
                s = KMime::MDN::SentManually;
                // TODO set type to Failed as well
                //      and clear modifiers
            }

            if (MessageComposer::MessageFactoryNG::MDNConfirmMultipleRecipients(msg)) {
                mdnInfo = MessageComposer::MDNAdviceHelper::instance()->mdnMessageText("mdnMultipleAddressesInReceiptTo");
                s = KMime::MDN::SentManually;
            }

            if (MessageComposer::MessageFactoryNG::MDNReturnPathEmpty(msg)) {
                mdnInfo = MessageComposer::MDNAdviceHelper::instance()->mdnMessageText("mdnReturnPathEmpty");
                s = KMime::MDN::SentManually;
            }

            if (MessageComposer::MessageFactoryNG::MDNReturnPathNotInRecieptTo(msg)) {
                mdnInfo = MessageComposer::MDNAdviceHelper::instance()->mdnMessageText("mdnReturnPathNotInReceiptTo");
                s = KMime::MDN::SentManually;
            }

            if (MessageComposer::MessageFactoryNG::MDNRequested(msg)) {
                if (s != KMime::MDN::SentManually) {
                    // don't ask again if user has already been asked. use the users' decision
                    mdnInfo = MessageComposer::MDNAdviceHelper::instance()->mdnMessageText("mdnNormalAsk");
                }
            } else { // if message doesn't have a disposition header, never send anything.
                mode = 0;
            }
        }
        result = true;
    } else {
        qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to start job";
    }
    Q_EMIT showMdnInfo(mdnInfo, s);
    deleteLater();
    return result;
}

const Akonadi::Item &MDNWarningWidgetJob::item() const
{
    return mItem;
}

void MDNWarningWidgetJob::setItem(const Akonadi::Item &newItem)
{
    mItem = newItem;
}

#include "moc_mdnwarningwidgetjob.cpp"
