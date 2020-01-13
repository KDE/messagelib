/*  -*- c++ -*-
    interfaces/urlhandler.h

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003 Marc Mutz <mutz@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#ifndef KMAIL_INTERFACES_URLHANDLER_H
#define KMAIL_INTERFACES_URLHANDLER_H

#include <QUrl>

class QString;
class QPoint;

namespace MessasgeViewer {
class ViewerPrivate;
}

namespace MimeTreeParser {
/**
  * @short An interface to reader link handlers
  * @author Marc Mutz <mutz@kde.org>
  *
  */
class URLHandler
{
public:
    virtual ~URLHandler()
    {
    }

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
    virtual Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &url, MessageViewer::ViewerPrivate *w) const = 0;

    /**
     * Called when shift-clicking the link in the reader.
     * @return true if the click was handled by this URLHandler, false otherwise
     */
    virtual Q_REQUIRED_RESULT bool handleShiftClick(const QUrl &url, MessageViewer::ViewerPrivate *window) const
    {
        Q_UNUSED(url);
        Q_UNUSED(window);
        return false;
    }

    /**
     * @return should return true if this URLHandler will handle the drag
     */
    virtual Q_REQUIRED_RESULT bool willHandleDrag(const QUrl &url, MessageViewer::ViewerPrivate *window) const
    {
        Q_UNUSED(url);
        Q_UNUSED(window);
        return false;
    }

    /**
     * Called when starting a drag with the given URL.
     * If the drag is handled, you should create a drag object.
     * @return true if the click was handled by this URLHandler, false otherwise
     */
    virtual Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, MessageViewer::ViewerPrivate *window) const
    {
        Q_UNUSED(url);
        Q_UNUSED(window);
        return false;
    }
};
}

#endif // KMAIL_INTERFACES_URLHANDLER_H
