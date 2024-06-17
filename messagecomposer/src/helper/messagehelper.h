/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <Akonadi/Item>
#include <KMime/Headers>
#include <KMime/Message>

namespace KIdentityManagementCore
{
class IdentityManager;
}

namespace KMime
{
class Message;
}

/**
 * Contains random helper methods when dealing with messages.
 * TODO: cleanup and organize, along with similar methods in messageviewer.
 */
namespace MessageHelper
{
/** Initialize header fields. Should be called on new messages
    if they are not set manually. E.g. before composing. Calling
    of setAutomaticFields(), see below, is still required. */
void MESSAGECOMPOSER_EXPORT initHeader(const KMime::Message::Ptr &message, const KIdentityManagementCore::IdentityManager *identMan, uint id = 0);

/** Set the from, to, cc, bcc, encryption etc headers as specified in the
 * given identity. */
void applyIdentity(const KMime::Message::Ptr &message, const KIdentityManagementCore::IdentityManager *identMan, uint id);

/** Initialize headers fields according to the identity and the transport
   header of the given original message */
void initFromMessage(const KMime::Message::Ptr &msg,
                     const KMime::Message::Ptr &orgiMsg,
                     KIdentityManagementCore::IdentityManager *,
                     uint id,
                     bool idHeaders = true);

MESSAGECOMPOSER_EXPORT KMime::Types::AddrSpecList extractAddrSpecs(const KMime::Message::Ptr &msg, const QByteArray &header);

/** Set fields that are either automatically set (Message-id)
    or that do not change from one message to another (MIME-Version).
    Call this method before sending *after* all changes to the message
    are done because this method does things different if there are
    attachments / multiple body parts. */
void setAutomaticFields(const KMime::Message::Ptr &msg, bool isMultipart = false);
}
