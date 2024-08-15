/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "messagecomposer_export.h"
#include "messagesender.h"

#include <QObject>

#include <KMime/Message>

class KJob;

namespace MessageComposer
{
class AkonadiSenderPrivate;
/**
 * @brief The AkonadiSender class
 */
class MESSAGECOMPOSER_EXPORT AkonadiSender : public QObject, public MessageSender
{
    Q_OBJECT

public:
    explicit AkonadiSender(QObject *parent = nullptr);
    ~AkonadiSender() override;

protected:
    /**
    Send given message. The message is either queued or sent
    immediately. The default behaviour, as selected with
    setSendImmediate(), can be overwritten with the parameter
    sendNow (by specifying true or false).
    The sender takes ownership of the given message on success,
    so DO NOT DELETE OR MODIFY the message further.
    Returns true on success.

    TODO cberzan: update docu...
    */
    [[nodiscard]] bool doSend(const KMime::Message::Ptr &msg, short sendNow) override;

    /**
    Send queued messages, using the specified transport or the
    default, if none is given.
    */
    [[nodiscard]] bool doSendQueued(int transportId = -1) override;

private:
    /**
    Queue or send immediately one message using MailTransport::MessageQueueJob.
    */
    MESSAGECOMPOSER_NO_EXPORT void sendOrQueueMessage(const KMime::Message::Ptr &msg,
                                                      MessageComposer::MessageSender::SendMethod method = MessageComposer::MessageSender::SendDefault);

    MESSAGECOMPOSER_NO_EXPORT void queueJobResult(KJob *job);
    std::unique_ptr<AkonadiSenderPrivate> const d;
};
}
