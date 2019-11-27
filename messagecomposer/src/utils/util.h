/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>
  Copyright (C) 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Leo Franchi <lfranchi@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef MESSAGECOMPOSER_UTIL_H
#define MESSAGECOMPOSER_UTIL_H

#include "messagecomposer_export.h"
#include "Libkleo/Enum"
#include <KMime/Message>
#include <AkonadiCore/Item>
#include <Akonadi/KMime/MessageStatus>

class QTextDocument;

namespace KMime {
class Content;
}

namespace MessageComposer {
namespace Util {
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QByteArray selectCharset(const QVector<QByteArray> &charsets, const QString &text);

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QStringList AttachmentKeywords();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString cleanedUpHeaderString(const QString &s);

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool sendMailDispatcherIsOnline(QWidget *parent = nullptr);
MESSAGECOMPOSER_EXPORT void removeNotNecessaryHeaders(const KMime::Message::Ptr &msg);

/**
 * find mimetype in message
 */
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT KMime::Content *findTypeInMessage(KMime::Content *data, const QByteArray &mimeType, const QByteArray &subType);

/**
   * Adds private headers to the given @p message that links it to the original message.
   *
   * @param message The message to add the link information to.
   * @param id The item id of the original message.
   * @param status The status (replied or forwarded) that links the message to the original message.
   */
MESSAGECOMPOSER_EXPORT void addLinkInformation(const KMime::Message::Ptr &message, Akonadi::Item::Id item, Akonadi::MessageStatus status);

/**
   * Reads the private headers of the given @p message to extract link information to its original message.
   *
   * @param message The message to read the link information from.
   * @param id Will contain the item id of the original message.
   * @param status Will contain the status (replied or forwarded) that linked the message to the original message.
   * @returns Whether the mail contains valid link information or not.
   */
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool getLinkInformation(const KMime::Message::Ptr &msg, QVector<Akonadi::Item::Id> &id, QVector<Akonadi::MessageStatus> &status);

/**
   * Returns whether the item represents a valid KMime::Message that is not
   * in the Akonadi store (yet). This happens when operating on messages
   * attached to other mails, for example. Such items are not "valid", in
   * the akonadi sense, since jobs cannot sensibly use them, but they do
   * contain a valid message pointer.
   */
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool isStandaloneMessage(const Akonadi::Item &item);

/**
   * Retrieve the KMime::Message from the item, if there is one.
   * @returns A valid message pointer, or 0, is the item does not contain
   * a valid message.
   */
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT KMime::Message::Ptr message(const Akonadi::Item &item);

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool hasMissingAttachments(const QStringList &attachmentKeywords, QTextDocument *doc, const QString &subj);
}
}

#endif
