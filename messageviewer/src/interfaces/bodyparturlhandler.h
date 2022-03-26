/*  -*- c++ -*-
    interfaces/bodyparturlhandler.h

    This file is part of KMail's plugin interface.
    SPDX-FileCopyrightText: 2003, 2004 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

class QString;
class QPoint;

namespace MimeTreeParser
{
namespace Interface
{
class BodyPart;
}
}

namespace MessageViewer
{
class Viewer;
}

namespace MessageViewer
{
namespace Interface
{
/**
 * @short An interface to body part reader link handlers
 * @author Marc Mutz <mutz@kde.org>
 *
 * This interface is a condensed of variant of the more general
 * @see URLHandler interface, designed to make bodypart-dependent
 * link operations possible without exposing KMail-internal
 * classes.
 *
 * Implementation-wise, these handlers are called as a nested
 * Chain Of Responsibility by an internal implementation of
 * URLHandler.
 *
 * You can create a link whose handling is passed to this handler
 * by using BodyPart::makeLink( const QString & path ). \a path is
 * what * is passed back to the methods of this interface.
 *
 * Note that the BodyPart interface does not provide a means of
 * learning the content type of the body part passed. This is
 * intentional. It is expected that either separate
 * BodyPartURLHandlers are created for these purposes or else the
 * information encoded into the path parameter by the
 * BodyPartFormatter.
 */
class BodyPartURLHandler
{
public:
    virtual ~BodyPartURLHandler() = default;

    virtual QString name() const = 0;
    /** Called when LMB-clicking on a link in the reader. Should
    start processing equivalent to "opening" the link.

    @return true if the click was handled by this handler, false
    otherwise.
    */
    virtual bool handleClick(MessageViewer::Viewer *viewerInstance, MimeTreeParser::Interface::BodyPart *part, const QString &path) const = 0;

    /** Called when RMB-clicking on a link in the reader. Should
    show a context menu at the specified point with the
    specified widget as parent.

    @return true if the right-click was handled by this handler,
    false otherwise.
    */
    virtual bool handleContextMenuRequest(MimeTreeParser::Interface::BodyPart *part, const QString &path, const QPoint &p) const = 0;

    /** Called when hovering over a link.

        @return a string to be shown in the status bar while
    hovering over this link or QString() if the link was not
    handled by this handler.
    */
    virtual QString statusBarMessage(MimeTreeParser::Interface::BodyPart *part, const QString &path) const = 0;
};
} // namespace Interface
}
