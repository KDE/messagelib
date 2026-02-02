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
/*!
 * \class MessageViewer::AttachmentStrategy
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/AttachmentStrategy
 * \brief The AttachmentStrategy class
 */
class MESSAGEVIEWER_EXPORT AttachmentStrategy
{
protected:
    /*!
     * \brief Constructor (protected)
     */
    AttachmentStrategy();
    /*!
     * \brief Destructor (virtual)
     */
    virtual ~AttachmentStrategy();

public:
    //
    // Factory methods:
    //
    enum Type {
        Iconic,
        Smart,
        Inlined,
        Hidden,
        HeaderOnly
    };

    /*!
     * \brief Creates an AttachmentStrategy of the given type
     * \param type the strategy type
     * \return const AttachmentStrategy pointer
     */
    static const AttachmentStrategy *create(Type type);
    /*!
     * \brief Creates an AttachmentStrategy of the given type name
     * \param type the strategy type name
     * \return const AttachmentStrategy pointer
     */
    static const AttachmentStrategy *create(const QString &type);

    /*!
     * \brief Returns the iconic attachment strategy
     * \return const AttachmentStrategy pointer
     */
    static const AttachmentStrategy *iconic();
    /*!
     * \brief Returns the smart attachment strategy
     * \return const AttachmentStrategy pointer
     */
    static const AttachmentStrategy *smart();
    /*!
     * \brief Returns the inlined attachment strategy
     * \return const AttachmentStrategy pointer
     */
    static const AttachmentStrategy *inlined();
    /*!
     * \brief Returns the hidden attachment strategy
     * \return const AttachmentStrategy pointer
     */
    static const AttachmentStrategy *hidden();
    /*!
     * \brief Returns the header only attachment strategy
     * \return const AttachmentStrategy pointer
     */
    static const AttachmentStrategy *headerOnly();

    //
    // Navigation methods:
    //

    /*!
     * \brief Returns the name of the strategy
     * \return strategy name
     */
    virtual const char *name() const = 0;

    //
    // Behavioral:
    //

    enum Display {
        None,
        AsIcon,
        Inline,
    };

    /*!
     * \brief Checks if nested messages should be inlined
     * \return true if nested messages should be inlined
     */
    virtual bool inlineNestedMessages() const = 0;
    /*!
     * \brief Returns the default display mode for an attachment
     * \param node the MIME content node
     * \return display mode
     */
    virtual Display defaultDisplay(KMime::Content *node) const = 0;
    /*!
     * \brief Checks if attachment list is required in the header
     * \return true if attachment list is required
     */
    virtual bool requiresAttachmentListInHeader() const;
};
}
