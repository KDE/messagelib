/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <messageviewer/headerstrategy.h>

namespace MessageViewer
{
class DefaultGrantleeHeaderStrategy : public HeaderStrategy
{
public:
    DefaultGrantleeHeaderStrategy();
    ~DefaultGrantleeHeaderStrategy() override;

public:
    Q_REQUIRED_RESULT const char *name() const override
    {
        return "defaultgrantlee";
    }

    Q_REQUIRED_RESULT DefaultPolicy defaultPolicy() const override
    {
        return Hide;
    }
};
}
