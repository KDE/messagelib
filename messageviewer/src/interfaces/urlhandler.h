/*  -*- c++ -*-
    interfaces/urlhandler.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QUrl>

class QString;
class QPoint;

namespace MessasgeViewer
{
class ViewerPrivate;
}

namespace MimeTreeParser
{
/**
 * @short An interface to reader link handlers
 * @author Marc Mutz <mutz@kde.org>
 *
 */
class URLHandler
{
public:
    virtual ~URLHandler() = default;

    /**
     * Called when LMB-clicking on a link in the reader. Should start
     * processing equivalent to "opening" the link.
     *
     * @return true if the click was handled by this URLHandler,
     *         false otherwise.
     */
    virtual bool handleClick(const QUrl &url, MessageViewer::ViewerPrivate *w) const = 0;

    /**
     * Called when RMB-clicking on a link in the reader. Should show
     * a context menu at the specified point with the specified
     * widget as parent.
     *
     * @return true if the right-click was handled by this
     * URLHandler, false otherwise.
     */
    virtual bool handleContextMenuRequest(const QUrl &url, const QPoint &p, MessageViewer::ViewerPrivate *w) const = 0;

    /**
     * Called when hovering over a link.
     *
     * @return a string to be shown in the status bar while hovering
     * over this link.
     */
    [[nodiscard]] virtual QString statusBarMessage(const QUrl &url, MessageViewer::ViewerPrivate *w) const = 0;

    /**
     * Called when shift-clicking the link in the reader.
     * @return true if the click was handled by this URLHandler, false otherwise
     */
    [[nodiscard]] virtual bool handleShiftClick(const QUrl &url, MessageViewer::ViewerPrivate *window) const
    {
        Q_UNUSED(url)
        Q_UNUSED(window)
        return false;
    }

    /**
     * @return should return true if this URLHandler will handle the drag
     */
    [[nodiscard]] virtual bool willHandleDrag(const QUrl &url, MessageViewer::ViewerPrivate *window) const
    {
        Q_UNUSED(url)
        Q_UNUSED(window)
        return false;
    }

    /**
     * Called when starting a drag with the given URL.
     * If the drag is handled, you should create a drag object.
     * @return true if the click was handled by this URLHandler, false otherwise
     */
    [[nodiscard]] virtual bool handleDrag(const QUrl &url, MessageViewer::ViewerPrivate *window) const
    {
        Q_UNUSED(url)
        Q_UNUSED(window)
        return false;
    }
};
}
