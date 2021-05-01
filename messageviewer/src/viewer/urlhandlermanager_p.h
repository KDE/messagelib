/*
  SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "urlhandlermanager.h"
#include <MessageViewer/URLHandler>
#include <QHash>

namespace KMime
{
class Content;
}

namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT BodyPartURLHandlerManager : public MimeTreeParser::URLHandler
{
public:
    BodyPartURLHandlerManager()
        : MimeTreeParser::URLHandler()
    {
    }

    ~BodyPartURLHandlerManager() override;

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;

    void registerHandler(const Interface::BodyPartURLHandler *handler, const QString &mimeType);
    void unregisterHandler(const Interface::BodyPartURLHandler *handler);

private:
    Q_REQUIRED_RESULT QVector<const Interface::BodyPartURLHandler *> handlersForPart(KMime::Content *node) const;

    using BodyPartHandlerList = QHash<QByteArray, QVector<const Interface::BodyPartURLHandler *>>;
    BodyPartHandlerList mHandlers;
};

class MESSAGEVIEWER_TESTS_EXPORT KMailProtocolURLHandler : public MimeTreeParser::URLHandler
{
public:
    KMailProtocolURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~KMailProtocolURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &url, const QPoint &, ViewerPrivate *) const override
    {
        return url.scheme() == QLatin1String("kmail");
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT ExpandCollapseQuoteURLManager : public MimeTreeParser::URLHandler
{
public:
    ExpandCollapseQuoteURLManager()
        : MimeTreeParser::URLHandler()
    {
    }

    ~ExpandCollapseQuoteURLManager() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT SMimeURLHandler : public MimeTreeParser::URLHandler
{
public:
    SMimeURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~SMimeURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT MailToURLHandler : public MimeTreeParser::URLHandler
{
public:
    MailToURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~MailToURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT ContactUidURLHandler : public MimeTreeParser::URLHandler
{
public:
    ContactUidURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~ContactUidURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT HtmlAnchorHandler : public MimeTreeParser::URLHandler
{
public:
    HtmlAnchorHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~HtmlAnchorHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return QString();
    }
};

class MESSAGEVIEWER_TESTS_EXPORT AttachmentURLHandler : public MimeTreeParser::URLHandler
{
public:
    AttachmentURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~AttachmentURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleShiftClick(const QUrl &, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;

private:
    KMime::Content *nodeForUrl(const QUrl &url, ViewerPrivate *w) const;
    bool attachmentIsInHeader(const QUrl &url) const;
};

class MESSAGEVIEWER_TESTS_EXPORT ShowAuditLogURLHandler : public MimeTreeParser::URLHandler
{
public:
    ShowAuditLogURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~ShowAuditLogURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
};

// Handler that prevents dragging of internal images added by KMail, such as the envelope image
// in the enterprise header
class MESSAGEVIEWER_TESTS_EXPORT InternalImageURLHandler : public MimeTreeParser::URLHandler
{
public:
    InternalImageURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~InternalImageURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return QString();
    }
};

class MESSAGEVIEWER_TESTS_EXPORT EmbeddedImageURLHandler : public MimeTreeParser::URLHandler
{
public:
    EmbeddedImageURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~EmbeddedImageURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &url, ViewerPrivate *) const override
    {
        Q_UNUSED(url)
        return QString();
    }
};

class MESSAGEVIEWER_TESTS_EXPORT KRunURLHandler : public MimeTreeParser::URLHandler
{
public:
    KRunURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~KRunURLHandler() override
    {
    }

    Q_REQUIRED_RESULT bool handleClick(const QUrl &, ViewerPrivate *) const override;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return QString();
    }
};
}
