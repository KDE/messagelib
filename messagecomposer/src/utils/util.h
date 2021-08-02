/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <Akonadi/KMime/MessageStatus>
#include <AkonadiCore/Item>
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
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT bool
getLinkInformation(const KMime::Message::Ptr &msg, QVector<Akonadi::Item::Id> &id, QVector<Akonadi::MessageStatus> &status);

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

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QStringList cleanEmailList(const QStringList &emails);
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QStringList cleanUpEmailListAndEncoding(const QStringList &emails);
MESSAGECOMPOSER_EXPORT void addCustomHeaders(const KMime::Message::Ptr &message, const QMap<QByteArray, QString> &customHeader);
}
}

