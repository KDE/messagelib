/*  -*- c++ -*-
    csshelper.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <MessageViewer/CSSHelperBase>

#include "messageviewer_export.h"

namespace MessageViewer
{
/**
 * @brief The CSSHelper class
 */
class MESSAGEVIEWER_EXPORT CSSHelper : public CSSHelperBase
{
public:
    explicit CSSHelper(const QPaintDevice *pd);
    ~CSSHelper() override;

    /** @return HTML head including style sheet definitions and the
    &gt;body&lt; tag */
    [[nodiscard]] QString htmlHead(const HtmlHeadSettings &htmlHeadSettings) const override;
    [[nodiscard]] QString endBodyHtml() const;
    void updateColor();
};
}
