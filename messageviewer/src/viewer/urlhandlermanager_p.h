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

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;

    void registerHandler(const Interface::BodyPartURLHandler *handler, const QString &mimeType);
    void unregisterHandler(const Interface::BodyPartURLHandler *handler);

private:
    [[nodiscard]] QList<const Interface::BodyPartURLHandler *> handlersForPart(KMime::Content *node) const;

    using BodyPartHandlerList = QHash<QByteArray, QList<const Interface::BodyPartURLHandler *>>;
    BodyPartHandlerList mHandlers;
};

class MESSAGEVIEWER_TESTS_EXPORT KMailProtocolURLHandler : public MimeTreeParser::URLHandler
{
public:
    KMailProtocolURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~KMailProtocolURLHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &url, const QPoint &, ViewerPrivate *) const override
    {
        return url.scheme() == QLatin1StringView("kmail");
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT ExpandCollapseQuoteURLManager : public MimeTreeParser::URLHandler
{
public:
    ExpandCollapseQuoteURLManager()
        : MimeTreeParser::URLHandler()
    {
    }

    ~ExpandCollapseQuoteURLManager() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT SMimeURLHandler : public MimeTreeParser::URLHandler
{
public:
    SMimeURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~SMimeURLHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT MailToURLHandler : public MimeTreeParser::URLHandler
{
public:
    MailToURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~MailToURLHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT ContactUidURLHandler : public MimeTreeParser::URLHandler
{
public:
    ContactUidURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~ContactUidURLHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *) const override;
    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT HtmlAnchorHandler : public MimeTreeParser::URLHandler
{
public:
    HtmlAnchorHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~HtmlAnchorHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return {};
    }
};

class MESSAGEVIEWER_TESTS_EXPORT AttachmentURLHandler : public MimeTreeParser::URLHandler
{
public:
    AttachmentURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~AttachmentURLHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleShiftClick(const QUrl &, ViewerPrivate *window) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    [[nodiscard]] bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;

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

    ~ShowAuditLogURLHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
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

    ~InternalImageURLHandler() override = default;

    [[nodiscard]] bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    [[nodiscard]] bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return {};
    }
};

class MESSAGEVIEWER_TESTS_EXPORT EmbeddedImageURLHandler : public MimeTreeParser::URLHandler
{
public:
    EmbeddedImageURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~EmbeddedImageURLHandler() override = default;

    [[nodiscard]] bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    [[nodiscard]] bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &url, ViewerPrivate *) const override
    {
        Q_UNUSED(url)
        return {};
    }
};

class MESSAGEVIEWER_TESTS_EXPORT KRunURLHandler : public MimeTreeParser::URLHandler
{
public:
    KRunURLHandler()
        : MimeTreeParser::URLHandler()
    {
    }

    ~KRunURLHandler() override = default;

    [[nodiscard]] bool handleClick(const QUrl &, ViewerPrivate *) const override;
    [[nodiscard]] bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    [[nodiscard]] QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return {};
    }
};
}
