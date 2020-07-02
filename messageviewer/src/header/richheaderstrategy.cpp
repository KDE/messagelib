/*  -*- c++ -*-
    header/headerstrategy.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#include "richheaderstrategy.h"

using namespace MessageViewer;

static const char *const richHeaders[] = {
    "subject", "date", "from", "cc", "bcc", "to",
    "organization", "organisation", "reply-to",
    "user-agent", "x-mailer", "x-bugzilla-url", "disposition-notification-to"
};
static const int numRichHeaders = sizeof richHeaders / sizeof *richHeaders;

RichHeaderStrategy::RichHeaderStrategy()
    : HeaderStrategy()
    , mHeadersToDisplay(stringList(richHeaders, numRichHeaders))
{
}

RichHeaderStrategy::~RichHeaderStrategy()
{
}
