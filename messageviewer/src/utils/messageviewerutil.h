/*
 * SPDX-FileCopyrightText: 2005 Till Adam <adam@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
 */

#pragma once

#include "messageviewer_export.h"
#include <Akonadi/Item>
#include <KMime/Content>
#include <KService>
#include <QStringConverter>
class QUrl;
class QWidget;
class QActionGroup;
class QAction;

namespace KMime
{
class Message;
}

namespace MessageViewer
{
/**
 * The Util namespace contains a collection of helper functions use in
 * various places.
 */
namespace Util
{
// return true if we should proceed, false if we should abort
[[nodiscard]] MESSAGEVIEWER_EXPORT bool containsExternalReferences(const QString &str, const QString &extraHead);
[[nodiscard]] bool MESSAGEVIEWER_EXPORT checkOverwrite(const QUrl &url, QWidget *w);
[[nodiscard]] MESSAGEVIEWER_EXPORT bool
saveMessageInMboxAndGetUrl(QUrl &url, const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages = false);
[[nodiscard]] MESSAGEVIEWER_EXPORT bool saveMessageInMbox(const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages = false);
[[nodiscard]] MESSAGEVIEWER_EXPORT bool excludeExtraHeader(const QString &s);
[[nodiscard]] MESSAGEVIEWER_EXPORT QString generateMboxFileName(const Akonadi::Item &msgBase);
[[nodiscard]] MESSAGEVIEWER_EXPORT bool saveAttachments(const KMime::Content::List &contents, QWidget *parent, QList<QUrl> &saveUrl);
/**
 * @brief Replaces the @c node message part by an empty attachment with information about deleted attachment.
 *
 * @param node A message part representing an attachment. The part will be replaced by a new empty party with
 *             filename "Deleted: <original attachment name>". Must not be @p null.
 * @return Returns whether the message was actually modified.
 */
[[nodiscard]] MESSAGEVIEWER_EXPORT bool deleteAttachment(KMime::Content *node);
/**
 * @brief Calls deleteAttachment() for each node in the @p contents list.
 *
 * @param contents List of attachments to replace by an empty part (see deleteAttachment())
 * @return Returns number of attachments that have actually been replaced.
 * @see deleteAttachment()
 */
[[nodiscard]] MESSAGEVIEWER_EXPORT int deleteAttachments(const KMime::Content::List &contents);

[[nodiscard]] MESSAGEVIEWER_EXPORT QAction *createAppAction(const KService::Ptr &service, bool singleOffer, QActionGroup *actionGroup, QObject *parent);
struct HtmlMessageInfo {
    QString htmlSource;
    QString extraHead;
    QString bodyStyle;
    [[nodiscard]] bool operator==(const HtmlMessageInfo &other) const
    {
        return other.htmlSource == htmlSource && other.extraHead == extraHead && other.bodyStyle == bodyStyle;
    }
};

[[nodiscard]] MESSAGEVIEWER_EXPORT HtmlMessageInfo processHtml(const QString &htmlSource);
[[nodiscard]] MESSAGEVIEWER_EXPORT QString generateFileNameForExtension(const Akonadi::Item &msgBase, const QString &extension);
[[nodiscard]] MESSAGEVIEWER_EXPORT QString parseBodyStyle(const QString &style);
}
}
Q_DECLARE_TYPEINFO(MessageViewer::Util::HtmlMessageInfo, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(MessageViewer::Util::HtmlMessageInfo)
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::Util::HtmlMessageInfo &t);
Q_DECLARE_METATYPE(KService::Ptr)
