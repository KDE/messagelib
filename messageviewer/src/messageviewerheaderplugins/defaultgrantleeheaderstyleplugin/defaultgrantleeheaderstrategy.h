/*
   SPDX-FileCopyrightText: 2015-2023 Laurent Montel <montel@kde.org>

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
    [[nodiscard]] const char *name() const override
    {
        return "defaultgrantlee";
    }

    [[nodiscard]] DefaultPolicy defaultPolicy() const override
    {
        return Hide;
    }
};
}
