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
#include <QTextCodec>
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
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool containsExternalReferences(const QString &str, const QString &extraHead);
Q_REQUIRED_RESULT bool MESSAGEVIEWER_EXPORT checkOverwrite(const QUrl &url, QWidget *w);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool
saveMessageInMboxAndGetUrl(QUrl &url, const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages = false);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool saveMessageInMbox(const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages = false);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool excludeExtraHeader(const QString &s);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QString generateMboxFileName(const Akonadi::Item &msgBase);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT bool saveAttachments(const KMime::Content::List &contents, QWidget *parent, QList<QUrl> &saveUrl);

Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QAction *createAppAction(const KService::Ptr &service, bool singleOffer, QActionGroup *actionGroup, QObject *parent);
/** Return a QTextCodec for the specified charset.
 * This function is a bit more tolerant, than QTextCodec::codecForName */
MESSAGEVIEWER_EXPORT const QTextCodec *codecForName(const QByteArray &_str);
MESSAGEVIEWER_EXPORT QStringDecoder decoderForName(const QByteArray &_str);
MESSAGEVIEWER_EXPORT QStringConverter::Encoding htmlEncoding(const QByteArray &data, const QByteArray &codec);
struct HtmlMessageInfo {
    QString htmlSource;
    QString extraHead;
    QString bodyStyle;
    Q_REQUIRED_RESULT bool operator==(const HtmlMessageInfo &other) const
    {
        return other.htmlSource == htmlSource && other.extraHead == extraHead && other.bodyStyle == bodyStyle;
    }
};

Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT HtmlMessageInfo processHtml(const QString &htmlSource);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QByteArray htmlCodec(const QByteArray &data, const QByteArray &currentCodec);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QString generateFileNameForExtension(const Akonadi::Item &msgBase, const QString &extension);
Q_REQUIRED_RESULT MESSAGEVIEWER_EXPORT QString parseBodyStyle(const QString &style);
}
}
Q_DECLARE_TYPEINFO(MessageViewer::Util::HtmlMessageInfo, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(MessageViewer::Util::HtmlMessageInfo)
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::Util::HtmlMessageInfo &t);
Q_DECLARE_METATYPE(KService::Ptr)
