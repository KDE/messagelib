/*  -*- c++ -*-
    urlhandlermanager.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"

#include <QVector>

class QUrl;
class QString;
class QPoint;

namespace MimeTreeParser
{
class URLHandler;
}

namespace MessageViewer
{
namespace Interface
{
class BodyPartURLHandler;
}
class ViewerPrivate;
class BodyPartURLHandlerManager;
/**
 * @short Singleton to manage the list of URLHandlers
 * @author Marc Mutz <mutz@kde.org>
 */
class MESSAGEVIEWER_TESTS_EXPORT URLHandlerManager
{
    static URLHandlerManager *self;

    URLHandlerManager();

public:
    ~URLHandlerManager();

    static URLHandlerManager *instance();

    void registerHandler(const MimeTreeParser::URLHandler *handler);
    void unregisterHandler(const MimeTreeParser::URLHandler *handler);

    void registerHandler(const Interface::BodyPartURLHandler *handler, const QString &mimeType);
    void unregisterHandler(const Interface::BodyPartURLHandler *handler);

    Q_REQUIRED_RESULT bool handleClick(const QUrl &url, ViewerPrivate *w = nullptr) const;
    Q_REQUIRED_RESULT bool handleShiftClick(const QUrl &url, ViewerPrivate *window = nullptr) const;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *w = nullptr) const;
    Q_REQUIRED_RESULT bool willHandleDrag(const QUrl &url, ViewerPrivate *window = nullptr) const;
    Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, ViewerPrivate *window = nullptr) const;
    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &url, ViewerPrivate *w = nullptr) const;

private:
    using HandlerList = QVector<const MimeTreeParser::URLHandler *>;
    HandlerList mHandlers;
    BodyPartURLHandlerManager *mBodyPartURLHandlerManager = nullptr;
};
}

