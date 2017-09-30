/*  -*- c++ -*-
    interfaces/htmlwriter.h

    This file is part of KMail's plugin interface.
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

#ifndef __MIMETREEPARSER_INTERFACES_HTMLWRITER_H__
#define __MIMETREEPARSER_INTERFACES_HTMLWRITER_H__

#include "mimetreeparser_export.h"

class QByteArray;
class QString;

namespace MimeTreeParser {
/**
  * @short An interface for HTML sinks.
  * @author Marc Mutz <mutz@kde.org>
  *
  */
class MIMETREEPARSER_EXPORT HtmlWriter
{
public:
    virtual ~HtmlWriter();

    /** Signal the begin of stuff to write, and give the CSS definitions */
    virtual void begin() = 0;
    /** Write out a chunk of text. No HTML escaping is performed. */
    virtual void write(const QString &html) = 0;
    /** Signal the end of stuff to write. */
    virtual void end() = 0;
    /**
     * Stop all possibly pending processing in order to be able to
     * call #begin() again.
     */
    virtual void reset();

    /**
      * Embed a part with Content-ID @p contentId, using url @p url.
      */
    virtual void embedPart(const QByteArray &contentId, const QString &url) = 0;

    virtual void extraHead(const QString &str) = 0;
};
}

#endif // __MIMETREEPARSER_INTERFACES_HTMLWRITER_H__
