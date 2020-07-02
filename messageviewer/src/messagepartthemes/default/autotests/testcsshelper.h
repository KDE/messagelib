/*
    SPDX-FileCopyrightText: 2013 Sandro Knauß <bugs@sandroknauss.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSSAGEVIEWER_TESTCSSHELPER_H
#define MESSSAGEVIEWER_TESTCSSHELPER_H

#include <MessageViewer/CSSHelper>

namespace MessageViewer {
namespace Test {
class TestCSSHelper : public MessageViewer::CSSHelper
{
public:
    explicit TestCSSHelper(const QPaintDevice *pd);
    ~TestCSSHelper() override;
    QString htmlHead(bool fixed) const override;
};
}
}

#endif // MESSSAGEVIEWER_TESTCSSHELPER_H
