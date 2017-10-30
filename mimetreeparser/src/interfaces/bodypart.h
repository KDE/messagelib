/*  -*- mode: C++; c-file-style: "gnu" -*-
    bodypart.h

    This file is part of KMail's plugin interface.
    Copyright (c) 2004 Marc Mutz <mutz@kde.org>,
                       Ingo Kloecker <kloecker@kde.org>

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

#ifndef __MIMETREEPARSER_INTERFACES_BODYPART_H__
#define __MIMETREEPARSER_INTERFACES_BODYPART_H__

#include "mimetreeparser_export.h"

namespace KMime {
class Content;
}

namespace MimeTreeParser {
class NodeHelper;
class ObjectTreeParser;
class ProcessResult;

namespace Interface {
class ObjectTreeSource;

/**
    @short interface of classes that implement status for BodyPartFormatters.
*/
class MIMETREEPARSER_EXPORT BodyPartMemento
{
public:
    virtual ~BodyPartMemento();

    virtual void detach() = 0;
};

/**
    @short interface of message body parts.
*/
class MIMETREEPARSER_EXPORT BodyPart
{
public:
    virtual ~BodyPart();

    /**
    @return the BodyPartMemento set for this part, or null, if
    none is set.
    */
    virtual BodyPartMemento *memento() const = 0;

    /** Returns the KMime::Content node represented here. Makes most of the above obsolete
        and probably should be used in the interfaces in the first place.
    */
    virtual KMime::Content *content() const = 0;

    /**
     * Returns the top-level content.
     * Note that this is _not_ necessarily the same as content()->topLevel(), for example the later
     * will not work for "extra nodes", i.e. nodes in encrypted parts of the mail.
     * topLevelContent() will return the correct result in this case. Also note that
     * topLevelContent()
     */
    virtual KMime::Content *topLevelContent() const = 0;

    /**
     * Ok, this is ugly, exposing the node helper here, but there is too much useful stuff in there
     * for real-world plugins. Still, there should be a nicer way for this.
     */
    virtual MimeTreeParser::NodeHelper *nodeHelper() const = 0;

    /**
     * For making it easier to refactor, add objectTreeParser
     */
    virtual MimeTreeParser::ObjectTreeParser *objectTreeParser() const = 0;
    virtual MimeTreeParser::Interface::ObjectTreeSource *source() const = 0;
    virtual MimeTreeParser::ProcessResult *processResult() const = 0;
};
} // namespace Interface
}

#endif // __MIMETREEPARSER_INTERFACES_BODYPART_H__
