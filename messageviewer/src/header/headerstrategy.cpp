/*  -*- c++ -*-
    headerstrategy.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "headerstrategy.h"

//
// HeaderStrategy abstract base:
//
using namespace MessageViewer;
HeaderStrategy::HeaderStrategy()
{
}

HeaderStrategy::~HeaderStrategy()
{
}

QStringList HeaderStrategy::headersToDisplay() const
{
    return QStringList();
}

QStringList HeaderStrategy::headersToHide() const
{
    return QStringList();
}

bool HeaderStrategy::showHeader(const QString &header) const
{
    const QString headerLower(header.toLower());
    if (headersToDisplay().contains(headerLower)) {
        return true;
    }
    if (headersToHide().contains(headerLower)) {
        return false;
    }
    return defaultPolicy() == Display;
}

QStringList HeaderStrategy::stringList(const char *const headers[], int numHeaders)
{
    QStringList sl;
    sl.reserve(numHeaders);
    for (int i = 0; i < numHeaders; ++i) {
        sl.push_back(QLatin1String(headers[i]));
    }
    return sl;
}
