/* SPDX-FileCopyrightText: 2009 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include "kmime/kmime_header_parsing.h"
#include "kmime/kmime_message.h"
#include "messagecore_export.h"

#include <QStringList>
#include <QVector>
class QUrl;

namespace KMime
{
namespace Types
{
struct Address;
using AddressList = QVector<Address>;
class Mailbox;
}
namespace Headers
{
namespace Generics
{
class MailboxList;
class AddressList;
}
}
}

namespace MessageCore
{
/**
 * This namespace contain helper functions for string manipulation
 */
namespace StringUtil
{
/**
 * Parses a mailto: url and extracts the information in the QMap (field name as key).
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QVector<QPair<QString, QString>> parseMailtoUrl(const QUrl &url);

/**
 * Strips the signature blocks from a message text. "-- " is considered as a signature block separator.
 *
 * @param message The message to remove the signature block from.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString stripSignature(const QString &message);

/**
 * Splits the given address list @p text into separate addresses.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT KMime::Types::AddressList splitAddressField(const QByteArray &text);

/**
 * Generates the Message-Id. It uses either the Message-Id @p suffix
 * defined by the user or the given email address as suffix. The @p address
 * must be given as addr-spec as defined in RFC 2822.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString generateMessageId(const QString &address, const QString &suffix);

/**
 * Quotes the following characters which have a special meaning in HTML:
 * '<'  '>'  '&'  '"'. Additionally '\\n' is converted to "<br />" if
 * @p removeLineBreaks is false. If @p removeLineBreaks is true, then
 * '\\n' is removed. Last but not least '\\r' is removed.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString quoteHtmlChars(const QString &text, bool removeLineBreaks = false);

/**
 * Removes all private header fields (e.g. *Status: and X-KMail-*) from the given @p message.
 * if cleanUpHeader is false don't remove X-KMail-Identity and X-KMail-Dictionary which is useful when we want restore mail.
 */
MESSAGECORE_EXPORT void removePrivateHeaderFields(const KMime::Message::Ptr &message, bool cleanUpHeader = true);

/**
 * Returns the @p message contents with the headers that should not be sent stripped off.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QByteArray asSendableString(const KMime::Message::Ptr &message);

/**
 * Return the message header with the headers that should not be sent stripped off.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QByteArray headerAsSendableString(const KMime::Message::Ptr &message);

/**
 * Used to determine if the visible part of the anchor contains
 * only the name part and not the given emailAddr or the full address.
 */
enum Display { DisplayNameOnly, DisplayFullAddress };

/**
 * Used to determine if the address should be a link or not.
 */
enum Link { ShowLink, HideLink };

/**
 * Used to determine if the address field should be expandable/collapsable.
 */
enum AddressMode { ExpandableAddresses, FullAddresses };

/**
 * Converts the email address(es) to (a) nice HTML mailto: anchor(s).
 * @p display determines if only the name part or the entire address should be returned.
 * @p cssStyle a custom css template.
 * @p link determines if the result should be a html link or not.
 * @p expandable determines if a long list of addresses should be expandable or shown
 * in full.
 * @p fieldName the name that the divs should be based on if expandable is set to ExpanableAddesses.
 * @p The number of addresses to show before collapsing the rest, if expandable is set to
 * ExpandableAddresses.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString emailAddrAsAnchor(const KMime::Headers::Generics::MailboxList *mailboxList,
                                                               Display display = DisplayNameOnly,
                                                               const QString &cssStyle = QString(),
                                                               Link link = ShowLink,
                                                               AddressMode expandable = FullAddresses,
                                                               const QString &fieldName = QString(),
                                                               int collapseNumber = 4);

/**
 * Same as above method, only for AddressList headers.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString emailAddrAsAnchor(const KMime::Headers::Generics::AddressList *addressList,
                                                               Display display = DisplayNameOnly,
                                                               const QString &cssStyle = QString(),
                                                               Link link = ShowLink,
                                                               AddressMode expandable = FullAddresses,
                                                               const QString &fieldName = QString(),
                                                               int collapseNumber = 4);

/**
 * Same as the above, only for Mailbox::List types.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString emailAddrAsAnchor(const QVector<KMime::Types::Mailbox> &mailboxList,
                                                               Display display = DisplayNameOnly,
                                                               const QString &cssStyle = QString(),
                                                               Link link = ShowLink,
                                                               AddressMode expandable = FullAddresses,
                                                               const QString &fieldName = QString(),
                                                               int collapseNumber = 4);

/**
 * Returns true if the given address is contained in the given address list.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT bool addressIsInAddressList(const QString &address, const QStringList &addresses);

/**
 * Uses the hostname as domain part and tries to determine the real name
 * from the entries in the password file.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString guessEmailAddressFromLoginName(const QString &userName);

/**
 *  Relayouts the given string so that the individual lines don't exceed the given
 *  maximal length.
 *
 *  As the name of the function implies, it is smart, which means it deals with quoting
 *  correctly. This means if a line already starts with quote characters and needs to be
 *  broken, the same quote characters are prepended to the next line as well.
 *
 *  This does _not_ add new quote characters in front of every line, that is the responsibility
 *  of the caller.
 *
 *  @param message The string which it to be relayouted
 *  @param maxLineLength reformat text to be this amount of columns at maximum. Note that this
 *                       also includes the trailing \n!
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString smartQuote(const QString &message, int maxLineLength);

/**
 * Convert quote wildcards into the final quote prefix.
 * @param wildString the string to be converted
 * @param fromDisplayString displayable string of the from email address
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString formatQuotePrefix(const QString &wildString, const QString &fromDisplayString);

/**
 * Cleans a filename by replacing characters not allowed or wanted on the filesystem
 *  e.g. ':', '/', '\' with '_'
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString cleanFileName(const QString &fileName);

/**
 * Removes the forward and reply markes (e.g. Re: or Fwd:) from a @p subject string.
 * Additional markers to act on can be specified in the MessageCore::GlobalSettings
 * object.
 */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString stripOffPrefixes(const QString &subject);

MESSAGECORE_EXPORT void setEncodingFile(QUrl &url, const QString &encoding);

/** Check for prefixes @p prefixRegExps in #subject(). If none
      is found, @p newPrefix + ' ' is prepended to the subject and the
      resulting string is returned. If @p replace is true, any
      sequence of whitespace-delimited prefixes at the beginning of
      #subject() is replaced by @p newPrefix
  **/
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString cleanSubject(KMime::Message *msg, const QStringList &prefixRegExps, bool replace, const QString &newPrefix);

/** Return this mails subject, with all "forward" and "reply"
      prefixes removed */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString cleanSubject(KMime::Message *msg);

/** Return this mails subject, formatted for "forward" mails */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString forwardSubject(KMime::Message *msg);

/** Return this mails subject, formatted for "reply" mails */
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString replySubject(KMime::Message *msg);
/** Check for prefixes @p prefixRegExps in @p str. If none
      is found, @p newPrefix + ' ' is prepended to @p str and the
      resulting string is returned. If @p replace is true, any
      sequence of whitespace-delimited prefixes at the beginning of
      @p str is replaced by @p newPrefix.
  **/
Q_REQUIRED_RESULT MESSAGECORE_EXPORT QString replacePrefixes(const QString &str, const QStringList &prefixRegExps, bool replace, const QString &newPrefix);
}
}

