/*
    messagesender.h

    This file is part of KMail, the KDE mail client
    SPDX-FileCopyrightText: 2005 Klarälvdalens Datakonsult AB
    SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KMime/KMimeMessage>
#include <MailTransport/Transport>

namespace MessageComposer
{

/// This interface allows to send messages either immediately or queued and sent later.
class MessageSender
{
protected:
    virtual ~MessageSender() = default;

public:
    /// This enum describes the send methods that can be used to send a message.
    enum SendMethod {
        SendDefault, ///< Use the default send method defined by \p defaultSendMethod.
        SendImmediate = true, ///< Send the message immediately.
        SendLater = false, ///< Queue the message and send it later with \p sendQueued.
    };

    /// This enum describes where the email will be saved.
    enum SaveIn {
        SaveInNone, ///< Save nowhere
        SaveInDrafts, ///< Save in drafts folder
        SaveInTemplates, ///< Save in templates fodler
        SaveInOutbox, ///< Save in outbox folder
    };

    enum SpeciaValue {
        DefaultTransport = -1,
    };

    /// Send the given message.
    ///
    /// The message is either queued (\p method == QueueMessage) or sent
    /// immediately (\p method = SendImmediately).
    ///
    /// If \p method == DefaultMethod, the default behavior is defined by
    /// the result \p defaultSendMethod.
    ///
    /// The sender takes ownership of the given message on success, so
    /// do not delete or modify the message further.
    ///
    /// \returns true on success
    Q_REQUIRED_RESULT virtual bool send(const KMime::Message::Ptr &msg, SendMethod method = SendDefault) = 0;

    /// Start job to send queued messages.
    ///
    /// Optionally a transport can be specified that will be used as the
    /// default transport.
    ///
    /// @return true if the job could be started.
    Q_REQUIRED_RESULT virtual bool sendQueued(MailTransport::Transport::Id transportId = DefaultTransport) = 0;
};
}
