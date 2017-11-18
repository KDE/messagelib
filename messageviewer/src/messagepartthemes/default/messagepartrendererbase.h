/*
    Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

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

#ifndef __MESSAGEVIEWER_MESSAGEPARTRENDERERBASE_H__
#define __MESSAGEVIEWER_MESSAGEPARTRENDERERBASE_H__

#include "messageviewer_export.h"

#include <MimeTreeParser/MessagePart>

#include <memory>
#include <map>

#include <QSharedPointer>

namespace MessageViewer {
class CSSHelperBase;
class HtmlWriter;

class MESSAGEVIEWER_EXPORT RenderContext
{
public:
    virtual ~RenderContext();

    virtual CSSHelperBase *cssHelper() const = 0;
    template<typename T>
    inline bool renderWithFactory(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer)
    {
        return renderWithFactory(&T::staticMetaObject, msgPart, writer);
    }

    inline bool renderWithFactory(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer)
    {
        return renderWithFactory(msgPart->metaObject(), msgPart, writer);
    }

    virtual void renderSubParts(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter) = 0;

    virtual bool isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart) = 0;
    virtual MimeTreeParser::IconType displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart) = 0;
    virtual bool showEmoticons() const = 0;
    virtual bool showExpandQuotesMark() const = 0;
    virtual bool showOnlyOneMimePart() const = 0;
    virtual int levelQuote() const = 0;

protected:
    virtual bool renderWithFactory(const QMetaObject *mo, const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer) = 0;
};

class MESSAGEVIEWER_EXPORT MessagePartRendererBase
{
public:
    MessagePartRendererBase();
    virtual ~MessagePartRendererBase();
    virtual bool render(const MimeTreeParser::MessagePart::Ptr &, HtmlWriter *htmlWriter, RenderContext *context)
    const = 0;
};
}
#endif
