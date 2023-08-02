/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 * SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "akonadisender.h"

#include "messagecomposer_debug.h"

#include "MessageComposer/Util"
#include "helper/messagehelper.h"
#include "settings/messagecomposersettings.h"
#include "utils/util_p.h"

#include <Libkdepim/ProgressManager>

#include <Akonadi/DispatcherInterface>
#include <Akonadi/MessageQueueJob>
#include <KEmailAddress>
#include <KIdentityManagement/Identity>
#include <KIdentityManagement/IdentityManager>
#include <MailTransport/Transport>
#include <MailTransport/TransportManager>
#include <MessageCore/StringUtil>

using namespace KMime::Types;
using namespace KPIM;
using namespace MessageComposer;

namespace
{

QStringList addrSpecListToStringList(const AddrSpecList &addresses, bool allowEmpty = false)
{
    QStringList result;
    for (const auto &address : addresses) {
        const auto addressString = address.asString();
        if (allowEmpty || !addressString.isEmpty()) {
            result.append(addressString);
        }
    }
    return result;
}

void extractSenderToCCAndBcc(const KMime::Message::Ptr &aMsg, QString &sender, QStringList &to, QStringList &cc, QStringList &bcc)
{
    sender = aMsg->sender()->asUnicodeString();
    if (aMsg->headerByType("X-KMail-Recipients")) {
        // extended BCC handling to prevent TOs and CCs from seeing
        // BBC information by looking at source of an OpenPGP encrypted mail
        to = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "X-KMail-Recipients"));
        aMsg->removeHeader("X-KMail-Recipients");
    } else if (aMsg->headerByType("Resent-To")) {
        to = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Resent-To"));
        cc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Resent-Cc"));
        bcc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Resent-Bcc"));
    } else {
        to = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "To"));
        cc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Cc"));
        bcc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Bcc"));
    }
}
}

class MessageComposer::AkonadiSenderPrivate
{
public:
    enum Method {
        SendImmediately,
        Queue,
    };
    AkonadiSenderPrivate(AkonadiSender *parent)
        : q(parent)
    {
    }

    AkonadiSender *q;
    QSet<KJob *> mPendingJobs;
    int mCustomTransportId = -1;

    bool sendOrQueueMessage(const KMime::Message::Ptr &message, Method method);

    void queueJobResult(KJob *job);
};

AkonadiSender::AkonadiSender(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<MessageComposer::AkonadiSenderPrivate>(this))
{
}

AkonadiSender::~AkonadiSender() = default;

bool AkonadiSender::send(const KMime::Message::Ptr &message, MessageSender::SendMethod method)
{
    if (method == MessageSender::SendDefault) {
        method = MessageComposer::MessageComposerSettings::self()->sendImmediate() ? MessageSender::SendImmediate : MessageSender::SendLater;
    }
    if (method == MessageSender::SendImmediate) {
        return d->sendOrQueueMessage(message, MessageComposer::AkonadiSenderPrivate::SendImmediately);
    } else {
        return d->sendOrQueueMessage(message, MessageComposer::AkonadiSenderPrivate::Queue);
    }
}

bool AkonadiSender::sendQueued(MailTransport::Transport::Id customTransportId)
{
    qCDebug(MESSAGECOMPOSER_LOG) << "Sending queued message with custom transport:" << customTransportId;
    if (!MessageComposer::Util::sendMailDispatcherIsOnline()) {
        return false;
    }

    d->mCustomTransportId = customTransportId;

    Akonadi::DispatcherInterface dispatcher;
    if (d->mCustomTransportId == -1) {
        dispatcher.dispatchManually();
    } else {
        dispatcher.dispatchManualTransport(d->mCustomTransportId);
    }
    return true;
}

bool AkonadiSenderPrivate::sendOrQueueMessage(const KMime::Message::Ptr &message, Method method)
{
    Q_ASSERT(message);
    qCDebug(MESSAGECOMPOSER_LOG) << "KMime::Message: \n[\n" << message->encodedContent().left(1000) << "\n]\n";

    auto qjob = new Akonadi::MessageQueueJob(q);
    if (message->hasHeader("X-KMail-FccDisabled")) {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::Delete);
    } else if (auto hrd = message->headerByType("X-KMail-Fcc")) {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToCollection);
        const int sentCollectionId = hrd->asUnicodeString().toInt();
        qjob->sentBehaviourAttribute().setMoveToCollection(Akonadi::Collection(sentCollectionId));
    } else if (auto hrd = message->headerByType("X-KMail-Identity")) {
        KIdentityManagement::IdentityManager *im = KIdentityManagement::IdentityManager::self();
        const QString identityStrId = hrd->asUnicodeString();
        const KIdentityManagement::Identity id = im->modifyIdentityForUoid(identityStrId.toUInt());
        const QString fccId = id.fcc();
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToCollection);
        const int sentCollectionId = fccId.toInt();
        qjob->sentBehaviourAttribute().setMoveToCollection(Akonadi::Collection(sentCollectionId));
    } else if (auto hrd = message->headerByType("X-KMail-Identity-Name")) {
        KIdentityManagement::IdentityManager *im = KIdentityManagement::IdentityManager::self();
        const QString identityStrName = hrd->asUnicodeString();
        const KIdentityManagement::Identity id = im->modifyIdentityForName(identityStrName);
        const QString fccId = id.fcc();
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToCollection);
        const int sentCollectionId = fccId.toInt();
        qjob->sentBehaviourAttribute().setMoveToCollection(Akonadi::Collection(sentCollectionId));
    } else {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToDefaultSentCollection);
    }
    qjob->setMessage(message);

    // Get transport.
    int transportId = MessageComposer::MessageSender::DefaultTransport;
    if (mCustomTransportId != MessageComposer::MessageSender::DefaultTransport) {
        transportId = mCustomTransportId;
    } else {
        if (auto hrd = message->headerByType("X-KMail-Transport")) {
            transportId = hrd->asUnicodeString().toInt();
        }
    }
    const auto *transport = MailTransport::TransportManager::self()->transportById(transportId);
    if (!transport) {
        qCWarning(MESSAGECOMPOSER_LOG) << "No email transporter found with id" << transportId;
        qjob->deleteLater();
        return false;
    }

    if ((method == SendImmediately) && !MessageComposer::Util::sendMailDispatcherIsOnline()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Trying to send a message but dispatcher is offline";
        qjob->deleteLater();
        return false;
    }

    qCDebug(MESSAGECOMPOSER_LOG) << "Using transport (" << transport->name() << "," << transport->id() << ")";
    qjob->transportAttribute().setTransportId(transport->id());

    // if we want to manually queue it for sending later, then do it
    if (method == Queue) {
        qjob->dispatchModeAttribute().setDispatchMode(Akonadi::DispatchModeAttribute::Manual);
    }

    // Get addresses.
    QStringList to;
    QStringList cc;
    QStringList bcc;
    QString from;
    extractSenderToCCAndBcc(message, from, to, cc, bcc);
    qjob->addressAttribute().setFrom(from);
    qjob->addressAttribute().setTo(to);
    qjob->addressAttribute().setCc(cc);
    qjob->addressAttribute().setBcc(bcc);

    if (qjob->addressAttribute().to().isEmpty()) {
        qCWarning(MESSAGECOMPOSER_LOG) << " Impossible to specify TO! It's a bug";
        qjob->deleteLater();
        return false;
    }

    if (transport && transport->specifySenderOverwriteAddress()) {
        qjob->addressAttribute().setFrom(
            KEmailAddress::extractEmailAddress(KEmailAddress::normalizeAddressesAndEncodeIdn(transport->senderOverwriteAddress())));
    } else {
        qjob->addressAttribute().setFrom(KEmailAddress::extractEmailAddress(KEmailAddress::normalizeAddressesAndEncodeIdn(message->from()->asUnicodeString())));
    }

    MessageComposer::Util::addSendReplyForwardAction(message, qjob);

    MessageCore::StringUtil::removePrivateHeaderFields(message, false);
    message->assemble();

    // Queue the message.
    qjob->connect(qjob, &Akonadi::MessageQueueJob::result, q, [this](KJob *job) {
        queueJobResult(job);
    });
    mPendingJobs.insert(qjob);
    qjob->start();
    qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob started.";

    // TODO potential problem:
    // The MDA finishes sending a message before I queue the next one, and
    // thinking it is finished, the progress item deletes itself.
    // Turn the MDA offline until everything is queued?
    return true;
}

void AkonadiSenderPrivate::queueJobResult(KJob *job)
{
    Q_ASSERT(mPendingJobs.contains(job));
    mPendingJobs.remove(job);

    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob failed with error" << job->errorString();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob success.";
    }
}
