/*
    bodypartformatterfactory.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"

#include <QList>
#include <memory>
namespace MimeTreeParser
{
namespace Interface
{
class BodyPartFormatter;
}

class BodyPartFormatterFactoryPrivate;

/** The place to obtain BodyPartFormatter candidates for a given mime type. */
class MIMETREEPARSER_EXPORT BodyPartFormatterFactory
{
public:
    BodyPartFormatterFactory();
    virtual ~BodyPartFormatterFactory();

    static BodyPartFormatterFactory *instance();

    /**
     *  Returns all suitable formatters for the given mimetype.
     *  The candidates are ordered by priority, with the catch-call
     *  formatter coming last.
     */
    [[nodiscard]] QList<const Interface::BodyPartFormatter *> formattersForType(const QString &mimeType) const;

protected:
    void insert(const QString &mimeType, const Interface::BodyPartFormatter *formatter, int priority);
    virtual void loadPlugins();

private:
    Q_DISABLE_COPY(BodyPartFormatterFactory)
    std::unique_ptr<BodyPartFormatterFactoryPrivate> const d;
    friend class BodyPartFormatterFactoryPrivate;
};
}
