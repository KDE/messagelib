/*
    SPDX-FileCopyrightText: 2013 Sandro Knauß <bugs@sandroknauss.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <MessageViewer/CSSHelper>

namespace MessageViewer
{
namespace Test
{
class TestCSSHelper : public MessageViewer::CSSHelper
{
public:
    explicit TestCSSHelper(const QPaintDevice *pd);
    ~TestCSSHelper() override;
    QString htmlHead(const HtmlHeadSettings &htmlHeadSettings) const override;
};
}
}
