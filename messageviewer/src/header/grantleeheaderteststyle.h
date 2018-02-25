/*
   Copyright (C) 2013-2018 Laurent Montel <montel@kde.org>

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

#ifndef GRANTLEEHEADERTESTSTYLE_H
#define GRANTLEEHEADERTESTSTYLE_H

#include "headerstyle.h"
#include "messageviewer_export.h"

namespace MessageViewer {
class GrantleeHeaderTestStylePrivate;
class MESSAGEVIEWER_EXPORT GrantleeHeaderTestStyle : public HeaderStyle
{
    friend class GrantleeHeaderStyle;
public:
    GrantleeHeaderTestStyle();
    ~GrantleeHeaderTestStyle() override;

public:
    const char *name() const override;

    QString format(KMime::Message *message) const override;

    void setAbsolutePath(const QString &);
    void setMainFilename(const QString &);
    void setExtraDisplayHeaders(const QStringList &);

    bool hasAttachmentQuickList() const override;

private:
    GrantleeHeaderTestStylePrivate *const d;
};
}

#endif // GRANTLEEHEADERTESTSTYLE_H
