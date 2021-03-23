/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "headerstyle.h"
#include "messageviewer_export.h"

namespace MessageViewer
{
class GrantleeHeaderTestStylePrivate;
/**
 * @brief The GrantleeHeaderTestStyle class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT GrantleeHeaderTestStyle : public HeaderStyle
{
    friend class GrantleeHeaderStyle;

public:
    GrantleeHeaderTestStyle();
    ~GrantleeHeaderTestStyle() override;

public:
    const char *name() const override;

    Q_REQUIRED_RESULT QString format(KMime::Message *message) const override;

    void setAbsolutePath(const QString &);
    void setMainFilename(const QString &);
    void setExtraDisplayHeaders(const QStringList &);

private:
    GrantleeHeaderTestStylePrivate *const d;
};
}

