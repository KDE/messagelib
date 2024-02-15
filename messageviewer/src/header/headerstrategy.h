/*  -*- c++ -*-
    headerstrategy.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QStringList>

namespace MessageViewer
{
//
// Convenience function
//
/**
 * @brief The HeaderStrategy class
 */
class MESSAGEVIEWER_EXPORT HeaderStrategy
{
protected:
    HeaderStrategy();

public:
    virtual ~HeaderStrategy();
    //
    // Methods for handling the strategies:
    //
    virtual const char *name() const = 0;

    void readConfig();
    //
    // HeaderStrategy interface:
    //
    enum DefaultPolicy {
        Display,
        Hide,
    };

    [[nodiscard]] virtual QStringList headersToDisplay() const;
    [[nodiscard]] virtual QStringList headersToHide() const;
    [[nodiscard]] virtual DefaultPolicy defaultPolicy() const = 0;
    [[nodiscard]] virtual bool showHeader(const QString &header) const;
    [[nodiscard]] static QStringList stringList(const char *const headers[], int numHeaders);
};
}
