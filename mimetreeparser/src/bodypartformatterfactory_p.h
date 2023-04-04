/*  -*- mode: C++; c-file-style: "gnu" -*-
    bodypartformatterfactory.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QHash>
#include <vector>

namespace MimeTreeParser
{
class BodyPartFormatterFactory;

class BodyPartFormatterFactoryPrivate
{
public:
    explicit BodyPartFormatterFactoryPrivate(BodyPartFormatterFactory *factory);
    ~BodyPartFormatterFactoryPrivate();

    void setup();
    void messageviewer_create_builtin_bodypart_formatters(); // defined in bodypartformatter.cpp
    void insert(const QString &mimeType, const Interface::BodyPartFormatter *formatter, int priority = 0);
    void appendFormattersForType(const QString &mimeType, QList<const Interface::BodyPartFormatter *> &formatters);

    BodyPartFormatterFactory *const q;
    struct FormatterInfo {
        const Interface::BodyPartFormatter *formatter = nullptr;
        int priority = 0;
    };
    QHash<QString, std::vector<FormatterInfo>> registry;
};
}
