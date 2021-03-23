/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer/headerstyle.h"
#include "messageviewer_export.h"
namespace MessageViewer
{
class PlainHeaderStylePrivate;
/**
 * @brief The PlainHeaderStyle class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT PlainHeaderStyle : public HeaderStyle
{
public:
    PlainHeaderStyle();
    ~PlainHeaderStyle() override;

public:
    const char *name() const override;

    Q_REQUIRED_RESULT QString format(KMime::Message *message) const override;

private:
    PlainHeaderStylePrivate *const d;
};
}
