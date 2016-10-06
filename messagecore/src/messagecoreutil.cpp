/*
 * Copyright (C) 2015  Daniel Vrátil <dvratil@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "messagecoreutil.h"

#include <KColorScheme>
#include <QApplication>
#include <QDebug>

using namespace MessageCore;

static bool isLightTheme()
{
    return qApp->palette().color(QPalette::Background).value() >= 128;
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
        mQuoteLevel3DefaultTextColor = base.dark(200);
    } else {
        mQuoteLevel1DefaultTextColor = base.lighter(200);
        mQuoteLevel2DefaultTextColor = base.lighter(170);
        mQuoteLevel3DefaultTextColor = base.lighter(140);
    }

    mPgpEncryptedMessageColor = QColor(0x00, 0x80, 0xFF);
    mPgpEncryptedTextColor = QColor(0xFF, 0xFF, 0xFF); // white
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
