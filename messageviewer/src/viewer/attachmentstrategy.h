/*  -*- c++ -*-
    attachmentstrategy.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

class QString;
namespace KMime
{
class Content;
}

namespace MessageViewer
{
/**
 * @brief The AttachmentStrategy class
 */
class MESSAGEVIEWER_EXPORT AttachmentStrategy
{
protected:
    AttachmentStrategy();
    virtual ~AttachmentStrategy();

public:
    //
    // Factory methods:
    //
    enum Type { Iconic, Smart, Inlined, Hidden, HeaderOnly };

    static const AttachmentStrategy *create(Type type);
    static const AttachmentStrategy *create(const QString &type);

    static const AttachmentStrategy *iconic();
    static const AttachmentStrategy *smart();
    static const AttachmentStrategy *inlined();
    static const AttachmentStrategy *hidden();
    static const AttachmentStrategy *headerOnly();

    //
    // Navigation methods:
    //

    virtual const char *name() const = 0;

    //
    // Behavioral:
    //

    enum Display {
        None,
        AsIcon,
        Inline,
    };

    virtual bool inlineNestedMessages() const = 0;
    virtual Display defaultDisplay(KMime::Content *node) const = 0;
    virtual bool requiresAttachmentListInHeader() const;
};
}
