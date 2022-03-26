/*  -*- mode: C++; c-file-style: "gnu" -*-
    bodypartformatter.h

    This file is part of KMail's plugin interface.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"

#include <QSharedPointer>

#include "mimetreeparser/objecttreeparser.h"

namespace MimeTreeParser
{
class MessagePart;
using MessagePartPtr = QSharedPointer<MessagePart>;

namespace Interface
{
class BodyPart;
/**
 * @brief The BodyPartFormatter class
 */
class MIMETREEPARSER_EXPORT BodyPartFormatter
{
public:
    virtual ~BodyPartFormatter();

    virtual MimeTreeParser::MessagePartPtr process(BodyPart &part) const = 0;
};

/**
    @short interface for BodyPartFormatter plugins

    The interface is queried by for types, subtypes, and the
    corresponding bodypart formatter, and the result inserted into
    the bodypart formatter factory.

    Subtype alone or both type and subtype may be "*", which is
    taken as a wildcard, so that e.g. type=text subtype=* matches
    any text subtype, but with lesser specificity than a concrete
    mimetype such as text/plain. type=* is only allowed when
    subtype=*, too.
*/
class MIMETREEPARSER_EXPORT BodyPartFormatterPlugin
{
public:
    virtual ~BodyPartFormatterPlugin();

    virtual const BodyPartFormatter *bodyPartFormatter(int idx) const = 0;
};
} // namespace Interface
}

Q_DECLARE_INTERFACE(MimeTreeParser::Interface::BodyPartFormatterPlugin, "org.kde.messageviewer.bodypartformatter/1.1")
