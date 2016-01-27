
/*  -*- c++ -*-
    urlhandlermanager.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003      Marc Mutz <mutz@kde.org>
    Copyright (C) 2002-2003, 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    Copyright (c) 2009 Andras Mantia <andras@kdab.net>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License, version 2, as
    published by the Free Software Foundation.

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

#include "urlhandlermanager.h"
#include "messageviewer_debug.h"
#include "messageviewer/urlhandler.h"

#include <QString>

using namespace MessageViewer;

URLHandlerManager *URLHandlerManager::self = 0;

//
//
// URLHandlerManager
//
//

URLHandlerManager::URLHandlerManager()
{
}

URLHandlerManager::~URLHandlerManager()
{
}

URLHandlerManager *URLHandlerManager::instance()
{
    if (!self) {
        self = new URLHandlerManager();
    }
    return self;
}

void URLHandlerManager::registerHandler(const URLHandler *handler)
{
    Q_UNUSED(handler);
}

void URLHandlerManager::unregisterHandler(const URLHandler *handler)
{
    Q_UNUSED(handler);
}

void URLHandlerManager::registerHandler(const Interface::BodyPartURLHandler *handler)
{
    Q_UNUSED(handler);
}

void URLHandlerManager::unregisterHandler(const Interface::BodyPartURLHandler *handler)
{
    Q_UNUSED(handler);
}

bool URLHandlerManager::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    Q_UNUSED(url);
    Q_UNUSED(w);
    return false;
}

bool URLHandlerManager::handleShiftClick(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(url);
    Q_UNUSED(window);
    return false;
}

bool URLHandlerManager::willHandleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(url);
    Q_UNUSED(window);
    return false;
}

bool URLHandlerManager::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(url);
    Q_UNUSED(window);
    return false;
}

bool URLHandlerManager::handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *w) const
{
    Q_UNUSED(url);
    Q_UNUSED(p);
    Q_UNUSED(w);
    return false;
}

QString URLHandlerManager::statusBarMessage(const QUrl &url, ViewerPrivate *w) const
{
    Q_UNUSED(url);
    Q_UNUSED(w);
    return QString();
}