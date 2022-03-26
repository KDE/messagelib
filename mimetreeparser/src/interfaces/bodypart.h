/*  -*- mode: C++; c-file-style: "gnu" -*-
    bodypart.h

    This file is part of KMail's plugin interface.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"

namespace KMime
{
class Content;
}

namespace MimeTreeParser
{
class NodeHelper;
class ObjectTreeParser;
class ProcessResult;

namespace Interface
{
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
