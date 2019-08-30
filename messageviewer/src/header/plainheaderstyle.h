/*
   Copyright (C) 2013-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PLAINHEADERSTYLE_H
#define PLAINHEADERSTYLE_H

#include "messageviewer/headerstyle.h"
#include "messageviewer_export.h"
namespace MessageViewer {
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
#endif // PLAINHEADERSTYLE_H
