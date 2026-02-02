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
 * \class MimeTreeParser::Interface::BodyPartMemento
 * \inmodule MimeTreeParser
 * \inheaderfile MimeTreeParser/BodyPartMemento
 *
 * @short interface of classes that implement status for BodyPartFormatters.
 */
class MIMETREEPARSER_EXPORT BodyPartMemento
{
public:
    /*!
     * \brief Destructor.
     */
    virtual ~BodyPartMemento();

    /*!
     * \brief Detach the memento.
     */
    virtual void detach() = 0;
};

/**
 * \class MimeTreeParser::Interface::BodyPart
 * \inmodule MimeTreeParser
 * \inheaderfile MimeTreeParser/BodyPart
 *
 * @short interface of message body parts.
 */
class MIMETREEPARSER_EXPORT BodyPart
{
public:
    /*!
     * \brief Destructor.
     */
    virtual ~BodyPart();

    /*!
     * \brief Get the BodyPartMemento set for this part.
     * \return the BodyPartMemento set for this part, or null, if
     * none is set.
     */
    virtual BodyPartMemento *memento() const = 0;

    /*!
     * \brief Get the KMime::Content node.
     * Returns the KMime::Content node represented here. Makes most of the above obsolete
     * and probably should be used in the interfaces in the first place.
     * \return The KMime::Content pointer.
     */
    virtual KMime::Content *content() const = 0;

    /*!
     * \brief Get the top-level content.
     * Returns the top-level content.
     * Note that this is _not_ necessarily the same as content()->topLevel(), for example the later
     * will not work for "extra nodes", i.e. nodes in encrypted parts of the mail.
     * topLevelContent() will return the correct result in this case. Also note that
     * topLevelContent()
     * \return The top-level KMime::Content pointer.
     */
    virtual KMime::Content *topLevelContent() const = 0;

    /*!
     * \brief Get the node helper.
     * Ok, this is ugly, exposing the node helper here, but there is too much useful stuff in there
     * for real-world plugins. Still, there should be a nicer way for this.
     * \return Pointer to the NodeHelper.
     */
    virtual MimeTreeParser::NodeHelper *nodeHelper() const = 0;

    /*!
     * \brief Get the object tree parser.
     * For making it easier to refactor, add objectTreeParser
     * \return Pointer to the ObjectTreeParser.
     */
    virtual MimeTreeParser::ObjectTreeParser *objectTreeParser() const = 0;
    /*!
     * \brief Get the object tree source.
     * \return Pointer to the ObjectTreeSource.
     */
    virtual MimeTreeParser::Interface::ObjectTreeSource *source() const = 0;
    /*!
     * \brief Get the process result.
     * \return Pointer to the ProcessResult.
     */
    virtual MimeTreeParser::ProcessResult *processResult() const = 0;
};
} // namespace Interface
}
