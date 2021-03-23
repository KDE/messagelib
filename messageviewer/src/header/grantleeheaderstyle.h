/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer/headerstyle.h"

namespace MessageViewer
{
class GrantleeHeaderStylePrivate;
/**
 * @brief The GrantleeHeaderStyle class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT GrantleeHeaderStyle : public HeaderStyle
{
public:
    GrantleeHeaderStyle();
    ~GrantleeHeaderStyle() override;

public:
    const char *name() const override;

    Q_REQUIRED_RESULT QString format(KMime::Message *message) const override;

private:
    GrantleeHeaderStylePrivate *const d;
};
}
