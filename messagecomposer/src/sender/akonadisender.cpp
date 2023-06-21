/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
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
#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <MailTransport/Transport>
#include <MailTransport/TransportManager>
#include <MessageCore/StringUtil>
using namespace KMime::Types;
using namespace KPIM;
using namespace MessageComposer;

static QStringList addrSpecListToStringList(const AddrSpecList &l, bool allowEmpty = false)
{
    QStringList result;
    for (AddrSpecList::const_iterator it = l.constBegin(), end = l.constEnd(); it != end; ++it) {
        const QString s = (*it).asString();
        if (allowEmpty || !s.isEmpty()) {
            result.push_back(s);
        }
    }
    return result;
}

static void extractSenderToCCAndBcc(const KMime::Message::Ptr &aMsg, QString &sender, QStringList &to, QStringList &cc, QStringList &bcc)
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

class MessageComposer::AkonadiSenderPrivate
{
public:
    AkonadiSenderPrivate() = default;

    QSet<KJob *> mPendingJobs;
    int mCustomTransportId = -1;
};

AkonadiSender::AkonadiSender(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::AkonadiSenderPrivate)
{
}

AkonadiSender::~AkonadiSender() = default;

bool AkonadiSender::doSend(const KMime::Message::Ptr &aMsg, short sendNow)
{
    if (sendNow == -1) {
        sendNow = MessageComposer::MessageComposerSettings::self()->sendImmediate(); // -1 == use default setting
    }
    if (sendNow) {
        sendOrQueueMessage(aMsg, MessageComposer::MessageSender::SendImmediate);
    } else {
        sendOrQueueMessage(aMsg, MessageComposer::MessageSender::SendLater);
    }
    return true;
}

bool AkonadiSender::doSendQueued(int customTransportId)
{
    qCDebug(MESSAGECOMPOSER_LOG) << "Sending queued message with custom transport:" << customTransportId;
    if (!MessageComposer::Util::sendMailDispatcherIsOnline()) {
        return false;
    }

    d->mCustomTransportId = customTransportId;

    auto dispatcher = new Akonadi::DispatcherInterface();
    if (d->mCustomTransportId == -1) {
        dispatcher->dispatchManually();
    } else {
        dispatcher->dispatchManualTransport(d->mCustomTransportId);
    }
    delete dispatcher;
    return true;
}

void AkonadiSender::sendOrQueueMessage(const KMime::Message::Ptr &message, MessageComposer::MessageSender::SendMethod method)
{
    Q_ASSERT(message);
    qCDebug(MESSAGECOMPOSER_LOG) << "KMime::Message: \n[\n" << message->encodedContent().left(1000) << "\n]\n";

    auto qjob = new Akonadi::MessageQueueJob(this);
    if (message->hasHeader("X-KMail-FccDisabled")) {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::Delete);
    } else if (auto hrd = message->headerByType("X-KMail-Fcc")) {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToCollection);
        const int sentCollectionId = hrd->asUnicodeString().toInt();
        qjob->sentBehaviourAttribute().setMoveToCollection(Akonadi::Collection(sentCollectionId));
    } else if (auto hrd = message->headerByType("X-KMail-Identity")) {
        KIdentityManagementCore::IdentityManager *im = KIdentityManagementCore::IdentityManager::self();
        const QString identityStrId = hrd->asUnicodeString();
        const KIdentityManagementCore::Identity id = im->modifyIdentityForUoid(identityStrId.toUInt());
        const QString fccId = id.fcc();
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToCollection);
        const int sentCollectionId = fccId.toInt();
        qjob->sentBehaviourAttribute().setMoveToCollection(Akonadi::Collection(sentCollectionId));
    } else if (auto hrd = message->headerByType("X-KMail-Identity-Name")) {
        KIdentityManagementCore::IdentityManager *im = KIdentityManagementCore::IdentityManager::self();
        const QString identityStrName = hrd->asUnicodeString();
        const KIdentityManagementCore::Identity id = im->modifyIdentityForName(identityStrName);
        const QString fccId = id.fcc();
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToCollection);
        const int sentCollectionId = fccId.toInt();
        qjob->sentBehaviourAttribute().setMoveToCollection(Akonadi::Collection(sentCollectionId));
    } else {
        qjob->sentBehaviourAttribute().setSentBehaviour(Akonadi::SentBehaviourAttribute::MoveToDefaultSentCollection);
    }
    qjob->setMessage(message);

    // Get transport.
    int transportId = -1;
    if (d->mCustomTransportId != -1) {
        transportId = d->mCustomTransportId;
    } else {
        if (auto hrd = message->headerByType("X-KMail-Transport")) {
            transportId = hrd->asUnicodeString().toInt();
        }
    }
    const auto *transport = MailTransport::TransportManager::self()->transportById(transportId);
    if (!transport) {
        qCDebug(MESSAGECOMPOSER_LOG) << " No transport defined. Need to create it";
        qjob->deleteLater();
        return;
    }

    if ((method == MessageComposer::MessageSender::SendImmediate) && !MessageComposer::Util::sendMailDispatcherIsOnline()) {
        qjob->deleteLater();
        return;
    }

    qCDebug(MESSAGECOMPOSER_LOG) << "Using transport (" << transport->name() << "," << transport->id() << ")";
    qjob->transportAttribute().setTransportId(transport->id());

    // if we want to manually queue it for sending later, then do it
    if (method == MessageComposer::MessageSender::SendLater) {
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
        return;
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
    connect(qjob, &Akonadi::MessageQueueJob::result, this, &AkonadiSender::queueJobResult);
    d->mPendingJobs.insert(qjob);
    qjob->start();
    qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob started.";

    // TODO potential problem:
    // The MDA finishes sending a message before I queue the next one, and
    // thinking it is finished, the progress item deletes itself.
    // Turn the MDA offline until everything is queued?
}

void AkonadiSender::queueJobResult(KJob *job)
{
    Q_ASSERT(d->mPendingJobs.contains(job));
    d->mPendingJobs.remove(job);

    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob failed with error" << job->errorString();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob success.";
    }
}

#include "moc_akonadisender.cpp"
