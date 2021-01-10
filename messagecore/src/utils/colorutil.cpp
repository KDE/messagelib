/*
 * SPDX-FileCopyrightText: 2015 Daniel Vrátil <dvratil@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#include "colorutil.h"

#include <KColorScheme>
#include <QApplication>

using namespace MessageCore;

static bool isLightTheme()
{
    return qApp->palette().color(QPalette::Window).value() >= 128;
}

Q_GLOBAL_STATIC(ColorUtil, s_self)

ColorUtil *ColorUtil::self()
{
    return s_self;
}

ColorUtil::ColorUtil()
{
    initializeColors();
}

void ColorUtil::updateColors()
{
    initializeColors();
}

void ColorUtil::initializeColors()
{
    KColorScheme scheme(QPalette::Active, KColorScheme::View);
    mMisspelledDefaultTextColor = scheme.foreground(KColorScheme::NegativeText).color().lighter();
    auto base = scheme.foreground(KColorScheme::PositiveText).color();
    if (isLightTheme()) {
        mQuoteLevel1DefaultTextColor = base.darker(120);
        mQuoteLevel2DefaultTextColor = base.darker(150);
        mQuoteLevel3DefaultTextColor = base.darker(200);
    } else {
        mQuoteLevel1DefaultTextColor = base.lighter(200);
        mQuoteLevel2DefaultTextColor = base.lighter(170);
        mQuoteLevel3DefaultTextColor = base.lighter(140);
    }

    if (isLightTheme()) {
        mPgpEncryptedMessageColor = QColor(0x00, 0x80, 0xFF).lighter(180);
        mPgpEncryptedTextColor = QColor(0x00, 0x80, 0xFF).darker(200);
    } else {
        mPgpEncryptedMessageColor = QColor(0x00, 0x80, 0xFF).darker(300);
        mPgpEncryptedTextColor = QColor(0x00, 0x80, 0xFF).lighter(170);
    }
    mPgpSignedTrustedMessageColor = scheme.background(KColorScheme::PositiveBackground).color();
    mPgpSignedTrustedTextColor = scheme.foreground(KColorScheme::PositiveText).color();
    mPgpSignedUntrustedMessageColor = scheme.background(KColorScheme::NeutralBackground).color();
    mPgpSignedUntrustedTextColor = scheme.foreground(KColorScheme::NeutralText).color();
    mPgpSignedBadMessageColor = scheme.background(KColorScheme::NegativeBackground).color();
    mPgpSignedBadTextColor = scheme.foreground(KColorScheme::NegativeText).color();
    mLinkColor = scheme.foreground(KColorScheme::LinkText).color();
}

QColor ColorUtil::misspelledDefaultTextColor() const
{
    return mMisspelledDefaultTextColor;
}

QColor ColorUtil::quoteLevel1DefaultTextColor() const
{
    return mQuoteLevel1DefaultTextColor;
}

QColor ColorUtil::quoteLevel2DefaultTextColor() const
{
    return mQuoteLevel2DefaultTextColor;
}

QColor ColorUtil::quoteLevel3DefaultTextColor() const
{
    return mQuoteLevel3DefaultTextColor;
}

QColor ColorUtil::pgpSignedTrustedMessageColor() const
{
    return mPgpSignedTrustedMessageColor;
}

QColor ColorUtil::pgpSignedTrustedTextColor() const
{
    return mPgpSignedTrustedTextColor;
}

QColor ColorUtil::pgpSignedUntrustedMessageColor() const
{
    return mPgpSignedUntrustedMessageColor;
}

QColor ColorUtil::pgpSignedUntrustedTextColor() const
{
    return mPgpSignedUntrustedTextColor;
}

QColor ColorUtil::pgpSignedBadMessageColor() const
{
    return mPgpSignedBadMessageColor;
}

QColor ColorUtil::pgpSignedBadTextColor() const
{
    return mPgpSignedBadTextColor;
}

QColor ColorUtil::pgpEncryptedMessageColor() const
{
    return mPgpEncryptedMessageColor;
}

QColor ColorUtil::pgpEncryptedTextColor() const
{
    return mPgpEncryptedTextColor;
}

QColor ColorUtil::linkColor() const
{
    return mLinkColor;
}
