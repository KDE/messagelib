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

#include <KMime/KMimeMessage>
#include <MailTransport/Transport>

class KJob;

namespace MessageComposer
{
class AkonadiSenderPrivate;

/// The AkonadiSender class
///
/// Implements MessageSender with Akonadi::MessageQueueJob and Akonadi::DispatcherInterface
class MESSAGECOMPOSER_EXPORT AkonadiSender : public QObject, public MessageSender
{
    Q_OBJECT

public:
    explicit AkonadiSender(QObject *parent = nullptr);
    ~AkonadiSender() override;

    Q_REQUIRED_RESULT bool send(const KMime::Message::Ptr &msg, MessageSender::SendMethod method = MessageSender::SendDefault) override;

    Q_REQUIRED_RESULT bool sendQueued(MailTransport::Transport::Id transportId = MessageSender::DefaultTransport) override;

private:
    std::unique_ptr<AkonadiSenderPrivate> const d;
};
}
