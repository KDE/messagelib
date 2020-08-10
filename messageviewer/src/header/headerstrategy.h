/*  -*- c++ -*-
    headerstrategy.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGEVIEWER_HEADERSTRATEGY_H
#define MESSAGEVIEWER_HEADERSTRATEGY_H

#include "messageviewer_export.h"
#include <QStringList>

namespace MessageViewer {
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
        Display, Hide
    };

    Q_REQUIRED_RESULT virtual QStringList headersToDisplay() const;
    Q_REQUIRED_RESULT virtual QStringList headersToHide() const;
    Q_REQUIRED_RESULT virtual DefaultPolicy defaultPolicy() const = 0;
    Q_REQUIRED_RESULT virtual bool showHeader(const QString &header) const;
    static QStringList stringList(const char *const headers[], int numHeaders);
};
}

#endif // MESSAGEVIEWER_HEADERSTRATEGY_H
