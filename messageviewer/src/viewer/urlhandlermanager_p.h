/*
  Copyright (C) 2017 Sandro Knauß <sknauss@kde.org>

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

#ifndef MESSAGEVIEWER_URLHANDLERMANAGER_P_H
#define MESSAGEVIEWER_URLHANDLERMANAGER_P_H

#include "urlhandlermanager.h"
#include <MessageViewer/URLHandler>

namespace KMime {
class Content;
}

namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT BodyPartURLHandlerManager : public MimeTreeParser::URLHandler
{
public:
    BodyPartURLHandlerManager() : MimeTreeParser::URLHandler()
    {
    }

    ~BodyPartURLHandlerManager() override;

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;

    void registerHandler(const Interface::BodyPartURLHandler *handler, const QString &mimeType);
    void unregisterHandler(const Interface::BodyPartURLHandler *handler);

private:
    QVector<const Interface::BodyPartURLHandler *> handlersForPart(KMime::Content *node) const;

    typedef QHash<QByteArray, QVector<const Interface::BodyPartURLHandler *> > BodyPartHandlerList;
    BodyPartHandlerList mHandlers;
};

class MESSAGEVIEWER_TESTS_EXPORT KMailProtocolURLHandler : public MimeTreeParser::URLHandler
{
public:
    KMailProtocolURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~KMailProtocolURLHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleContextMenuRequest(const QUrl &url, const QPoint &, ViewerPrivate *) const override
    {
        return url.scheme() == QLatin1String("kmail");
    }

    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT ExpandCollapseQuoteURLManager : public MimeTreeParser::URLHandler
{
public:
    ExpandCollapseQuoteURLManager() : MimeTreeParser::URLHandler()
    {
    }

    ~ExpandCollapseQuoteURLManager() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT SMimeURLHandler : public MimeTreeParser::URLHandler
{
public:
    SMimeURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~SMimeURLHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT MailToURLHandler : public MimeTreeParser::URLHandler
{
public:
    MailToURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~MailToURLHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT ContactUidURLHandler : public MimeTreeParser::URLHandler
{
public:
    ContactUidURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~ContactUidURLHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *) const override;
    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
};

class MESSAGEVIEWER_TESTS_EXPORT HtmlAnchorHandler : public MimeTreeParser::URLHandler
{
public:
    HtmlAnchorHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~HtmlAnchorHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return QString();
    }
};

class MESSAGEVIEWER_TESTS_EXPORT AttachmentURLHandler : public MimeTreeParser::URLHandler
{
public:
    AttachmentURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~AttachmentURLHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleShiftClick(const QUrl &, ViewerPrivate *window) const override;
    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
private:
    KMime::Content *nodeForUrl(const QUrl &url, ViewerPrivate *w) const;
    bool attachmentIsInHeader(const QUrl &url) const;
};

class MESSAGEVIEWER_TESTS_EXPORT ShowAuditLogURLHandler : public MimeTreeParser::URLHandler
{
public:
    ShowAuditLogURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~ShowAuditLogURLHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override;
    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override;
    bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
};

// Handler that prevents dragging of internal images added by KMail, such as the envelope image
// in the enterprise header
class MESSAGEVIEWER_TESTS_EXPORT InternalImageURLHandler : public MimeTreeParser::URLHandler
{
public:
    InternalImageURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~InternalImageURLHandler() override
    {
    }

    bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return QString();
    }
};

class MESSAGEVIEWER_TESTS_EXPORT EmbeddedImageURLHandler : public MimeTreeParser::URLHandler
{
public:
    EmbeddedImageURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~EmbeddedImageURLHandler() override
    {
    }

    bool handleDrag(const QUrl &url, ViewerPrivate *window) const override;
    bool willHandleDrag(const QUrl &url, ViewerPrivate *window) const override;
    bool handleClick(const QUrl &, ViewerPrivate *) const override
    {
        return false;
    }

    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    QString statusBarMessage(const QUrl &url, ViewerPrivate *) const override
    {
        Q_UNUSED(url);
        return QString();
    }
};

class MESSAGEVIEWER_TESTS_EXPORT KRunURLHandler : public MimeTreeParser::URLHandler
{
public:
    KRunURLHandler() : MimeTreeParser::URLHandler()
    {
    }

    ~KRunURLHandler() override
    {
    }

    bool handleClick(const QUrl &, ViewerPrivate *) const override;
    bool handleContextMenuRequest(const QUrl &, const QPoint &, ViewerPrivate *) const override
    {
        return false;
    }

    QString statusBarMessage(const QUrl &, ViewerPrivate *) const override
    {
        return QString();
    }
};
}
#endif // MESSAGEVIEWER_URLHANDLERMANAGER_P_H
