/*  -*- c++ -*-
    header/headerstrategy.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messageviewer/headerstrategy.h"
#include "messageviewer_export.h"
#include <QStringList>
//
namespace MessageViewer
{
//
// RichHeaderStrategy:
//   Date, Subject, From, To, CC, ### what exactly?
//
/**
 * @brief The RichHeaderStrategy class
 */
class MESSAGEVIEWER_EXPORT RichHeaderStrategy : public HeaderStrategy
{
public:
    RichHeaderStrategy();
    ~RichHeaderStrategy() override;

public:
    [[nodiscard]] const char *name() const override
    {
        return "rich";
    }

    [[nodiscard]] QStringList headersToDisplay() const override
    {
        return mHeadersToDisplay;
    }

    [[nodiscard]] DefaultPolicy defaultPolicy() const override
    {
        return Hide;
    }

private:
    const QStringList mHeadersToDisplay;
};
}
