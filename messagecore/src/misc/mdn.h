/*  -*- c++ -*-
    mdn.h

    KMime, the KDE Internet mail/usenet news message library.
    SPDX-FileCopyrightText: 2002 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"
#include <QList>
#include <QString>

class QByteArray;

namespace MessageCore
{

/*!
 * \namespace MessageCore::MDN
 * \inmodule MessageCore
 * \inheaderfile MessageCore/MDN
 */
namespace MDN
{

/*!
  \value Displayed The message has been displayed by the UA to someone reading the recipient's mailbox.  There is no guarantee that the content has been read or
  understood.
  \value Read
  \value Dispatched The message has been sent somewhere in some manner
  (e.g., printed, faxed, forwarded) without necessarily having been previously
  displayed to the user.  The user may or may not see the message later.
  \value Forwarded
  \value Processed The message has been processed in some manner (i.e., by
  some sort of rules or server) without being displayed to the user.  The user
  may or may not see the message later, or there may not even be a human user
  associated with the mailbox.
  \value Deleted The message has been deleted.  The recipient may or may not
  have seen the message.  The recipient might "undelete" the message at a
  later time and read the message.
  \value Denied The recipient does not wish the sender to be informed of the
  message's disposition.  A UA may also silently ignore message disposition
  requests in this situation.
  \value Failed on Disposition-Notification-Options containing
  unknown required options. ( == any required options )
  \value Failed A failure occurred that prevented the proper generation
  of an MDN.  More information about the cause of the failure may be contained
  in a Failure field.  The "failed" disposition type is not to be used for
  the situation in which there is is some problem in processing the message
  other than interpreting the request for an MDN.  The "processed" or other
  disposition type with appropriate disposition modifiers is to be used in
  such situations.
*/
enum DispositionType {
    Displayed,
    Read = Displayed,
    Deleted,
    Dispatched,
    Forwarded = Dispatched,
    Processed,
    Denied,
    Failed
};

/*!
  \value Error An error of some sort occurred that prevented
  successful processing of the message.  Further information is contained
  in an Error field.

  \value Warning The message was successfully processed but some
  sort of exceptional condition occurred.  Further information is contained
  in a Warning field.

  \value Superseded The message has been automatically rendered obsolete
  by another message received.  The recipient may still access and read the
  message later.

  \value Expired The message has reached its expiration date and has
  been automatically removed from the recipient's mailbox.

  \value MailboxTerminated The recipient's mailbox has been terminated and all
  message in it automatically removed.
*/
enum DispositionModifier {
    Error,
    Warning,
    Superseded,
    Expired,
    MailboxTerminated
};

/*!
  \value ManualAction The disposition described by the disposition type
  was a result of an explicit instruction by the user rather than some sort of
  automatically performed action.

  \value AutomaticAction The disposition described by the disposition type was
  a result of an automatic action, rather than an explicit instruction by the
  user for this message.
*/
enum ActionMode {
    ManualAction,
    AutomaticAction
};

/*!
  \value SentManually The user explicitly gave permission for this
  particular MDN to be sent.

  \value SentAutomatically The MDN was sent because the MUA had previously
  been configured to do so automatically.
*/
enum SendingMode {
    SentManually,
    SentAutomatically
};

/*!
  Generates the content of the message/disposition-notification body part.
*/
[[nodiscard]] MESSAGECORE_EXPORT QByteArray
dispositionNotificationBodyContent(const QString &finalRecipient,
                                   const QByteArray &originalRecipient,
                                   const QByteArray &originalMsgID,
                                   DispositionType disposition,
                                   ActionMode actionMode,
                                   SendingMode sendingMode,
                                   const QList<DispositionModifier> &dispositionModifers = QList<DispositionModifier>(),
                                   const QString &special = QString());

/*!
 *
 */
[[nodiscard]] MESSAGECORE_EXPORT QString descriptionFor(DispositionType d, const QList<DispositionModifier> &m = QList<DispositionModifier>());

} // namespace MDN

} // namespace MessageCore
