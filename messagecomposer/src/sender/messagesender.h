/*
    messagesender.h

    This file is part of KMail, the KDE mail client
    SPDX-FileCopyrightText: 2005 Klarälvdalens Datakonsult AB

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>
namespace MessageComposer
{
class MessageSender
{
protected:
    virtual ~MessageSender() = 0;

public:
    enum SendMethod {
        SendDefault = -1,
        SendImmediate = true,
        SendLater = false,
    };
    enum SaveIn {
        SaveInNone,
        SaveInDrafts,
        SaveInTemplates,
        SaveInOutbox,
    };
    /**
       Send given message.

       The message is either queued (@p method == SendLater) or sent
       immediately (@p method = SendImmediate). The default behaviour,
       as selected with setSendImmediate(), can be overwritten with the
       parameter @p method.  The sender takes ownership of the given
       message on success, so DO NOT DELETE OR MODIFY the message
       further.

       @return true on success.
    */
    [[nodiscard]] bool send(const KMime::Message::Ptr &msg, SendMethod method = SendDefault)
    {
        return doSend(msg, method);
    }

    /**
       Start sending all queued messages.

       FIXME: what does success mean here, if it's only _start_ sending?

       Optionally a transport can be specified that will be used as the
       default transport.

       @return true on success.
    */
    [[nodiscard]] bool sendQueued(int transportId = -1)
    {
        return doSendQueued(transportId);
    }

protected:
    [[nodiscard]] virtual bool doSend(const KMime::Message::Ptr &msg, short sendNow) = 0;
    [[nodiscard]] virtual bool doSendQueued(int transportId) = 0;
};

inline MessageSender::~MessageSender() = default;
}
