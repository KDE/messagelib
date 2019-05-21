/*
  Copyright (C) 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Andras Mantia <andras@kdab.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef KMAIL_MESSAGE_HELPER_H
#define KMAIL_MESSAGE_HELPER_H

#include "messagecomposer_export.h"

#include <kmime/kmime_headers.h>
#include <kmime/kmime_message.h>
#include <item.h>

namespace KIdentityManagement {
class IdentityManager;
}

namespace KMime {
class Message;
}

/**
 * Contains random helper methods when dealing with messages.
 * TODO: cleanup and organize, along with similar methods in messageviewer.
 */
namespace MessageHelper {
/** Initialize header fields. Should be called on new messages
    if they are not set manually. E.g. before composing. Calling
    of setAutomaticFields(), see below, is still required. */
void MESSAGECOMPOSER_EXPORT initHeader(const KMime::Message::Ptr &message, const KIdentityManagement::IdentityManager *identMan, uint id = 0);

/** Set the from, to, cc, bcc, encryption etc headers as specified in the
  * given identity. */
void applyIdentity(const KMime::Message::Ptr &message, const KIdentityManagement::IdentityManager *identMan, uint id);

/** Initialize headers fields according to the identity and the transport
   header of the given original message */
void initFromMessage(const KMime::Message::Ptr &msg, const KMime::Message::Ptr &orgiMsg, KIdentityManagement::IdentityManager *, uint id, bool idHeaders = true);

MESSAGECOMPOSER_EXPORT KMime::Types::AddrSpecList extractAddrSpecs(const KMime::Message::Ptr &msg, const QByteArray &header);

/** Set fields that are either automatically set (Message-id)
    or that do not change from one message to another (MIME-Version).
    Call this method before sending *after* all changes to the message
    are done because this method does things different if there are
    attachments / multiple body parts. */
void setAutomaticFields(const KMime::Message::Ptr &msg, bool isMultipart = false);
}

#endif
