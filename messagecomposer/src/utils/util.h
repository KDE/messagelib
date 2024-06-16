/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <Akonadi/Item>
#include <Akonadi/MessageStatus>
#include <KMime/Message>
#include <Libkleo/Enum>

class QTextDocument;

namespace KMime
{
class Content;
}

namespace MessageComposer
{
namespace Util
{
[[nodiscard]] MESSAGECOMPOSER_EXPORT QStringList AttachmentKeywords();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString cleanedUpHeaderString(const QString &s);

[[nodiscard]] MESSAGECOMPOSER_EXPORT bool sendMailDispatcherIsOnline(QWidget *parent = nullptr);

/**
 * find mimetype in message
 */
[[nodiscard]] MESSAGECOMPOSER_EXPORT KMime::Content *findTypeInMessage(KMime::Content *data, const QByteArray &mimeType, const QByteArray &subType);

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
[[nodiscard]] MESSAGECOMPOSER_EXPORT bool
getLinkInformation(const KMime::Message::Ptr &msg, QList<Akonadi::Item::Id> &id, QList<Akonadi::MessageStatus> &status);

/**
 * Returns whether the item represents a valid KMime::Message that is not
 * in the Akonadi store (yet). This happens when operating on messages
 * attached to other mails, for example. Such items are not "valid", in
 * the akonadi sense, since jobs cannot sensibly use them, but they do
 * contain a valid message pointer.
 */
[[nodiscard]] MESSAGECOMPOSER_EXPORT bool isStandaloneMessage(const Akonadi::Item &item);

/**
 * Retrieve the KMime::Message from the item, if there is one.
 * @returns A valid message pointer, or 0, is the item does not contain
 * a valid message.
 */
[[nodiscard]] MESSAGECOMPOSER_EXPORT KMime::Message::Ptr message(const Akonadi::Item &item);

[[nodiscard]] MESSAGECOMPOSER_EXPORT bool hasMissingAttachments(const QStringList &attachmentKeywords, QTextDocument *doc, const QString &subj);

[[nodiscard]] MESSAGECOMPOSER_EXPORT QStringList cleanEmailList(const QStringList &emails);
[[nodiscard]] MESSAGECOMPOSER_EXPORT QStringList cleanUpEmailListAndEncoding(const QStringList &emails);
MESSAGECOMPOSER_EXPORT void addCustomHeaders(const KMime::Message::Ptr &message, const QMap<QByteArray, QString> &customHeader);
}
}
