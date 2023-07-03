/*
    SPDX-FileCopyrightText: 2013 Sandro Knauß <bugs@sandroknauss.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testcsshelper.h"

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>

using namespace MessageViewer::Test;

TestCSSHelper::TestCSSHelper(const QPaintDevice *pd)
    : MessageViewer::CSSHelper(pd)
{
    mRecycleQuoteColors = false;
    mBackgroundColor = QColor(0xff, 0xff, 0xff);
    mForegroundColor = QColor(0x1f, 0x1c, 0x1b);
    mLinkColor = QColor(0x00, 0x57, 0xae);
    cPgpEncrH = QColor(0x00, 0x80, 0xff);
    cPgpOk1H = QColor(0x40, 0xff, 0x40);
    cPgpOk0H = QColor(0xff, 0xff, 0x40);
    cPgpWarnH = QColor(0xff, 0xff, 0x40);
    cPgpErrH = QColor(0xff, 0x00, 0x00);

    cPgpEncrHT = QColor(0xff, 0xff, 0xff);
    cPgpOk1HT = QColor(0x27, 0xae, 0x60);
    cPgpOk0HT = QColor(0xf6, 0x74, 0x00);
    cPgpWarnHT = QColor(0xf6, 0x74, 0x00);
    cPgpErrHT = QColor(0xda, 0x44, 0x53);

    cInlineMessage[Positive] = QColor(0, 255, 0);
    cInlineMessage[Information] = QColor(0, 0, 255);
    cInlineMessage[Warning] = QColor(255, 255, 0);
    cInlineMessage[Error] = QColor(255, 0, 0);

    for (int i = 0; i < 3; ++i) {
        mQuoteColor[i] = QColor(0x00, 0x80 - i * 0x10, 0x00);
    }

    QFont defaultFont = QFont(QStringLiteral("Sans Serif"), 9);
    mBodyFont = defaultFont;
    mPrintFont = defaultFont;
    mFixedFont = defaultFont;
    mFixedPrintFont = defaultFont;
    defaultFont.setItalic(true);
    mQuoteFont = defaultFont;

    mShrinkQuotes = false;

    QPalette pal;

    pal.setColor(QPalette::Window, QColor(0xd6, 0xd2, 0xd0));
    pal.setColor(QPalette::WindowText, QColor(0x22, 0x1f, 0x1e));
    pal.setColor(QPalette::Highlight, QColor(0x43, 0xac, 0xe8));
    pal.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
    pal.setColor(QPalette::Mid, QColor(0xb3, 0xab, 0xa7));

    QApplication::setPalette(pal);

    recalculatePGPColors();
}

TestCSSHelper::~TestCSSHelper() = default;

QString TestCSSHelper::htmlHead(const HtmlHeadSettings &htmlHeadSettings) const
{
    Q_UNUSED(htmlHeadSettings)
    return QStringLiteral(
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n"
        "<html>\n"
        "<body>\n");
}
