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

#ifndef MESSAGECORE_UTIL_H
#define MESSAGECORE_UTIL_H

#include "messagecore_export.h"

#include <QColor>

namespace MessageCore
{
class MESSAGECORE_EXPORT ColorUtil
{
public:
    ColorUtil();

    void updateColors();

    QColor misspelledDefaultTextColor() const;
    QColor quoteLevel1DefaultTextColor() const;
    QColor quoteLevel2DefaultTextColor() const;
    QColor quoteLevel3DefaultTextColor() const;
    QColor pgpSignedTrustedMessageColor() const;
    QColor pgpSignedTrustedTextColor() const;
    QColor pgpSignedUntrustedMessageColor() const;
    QColor pgpSignedUntrustedTextColor() const;
    QColor pgpSignedBadMessageColor() const;
    QColor pgpSignedBadTextColor() const;
    QColor pgpEncryptedMessageColor() const;
    QColor pgpEncryptedTextColor() const;

private:
    void initializeColors();
    QColor mMisspelledDefaultTextColor;
    QColor mQuoteLevel1DefaultTextColor;
    QColor mQuoteLevel2DefaultTextColor;
    QColor mQuoteLevel3DefaultTextColor;

    QColor mPgpSignedTrustedMessageColor;
    QColor mPgpSignedTrustedTextColor;
    QColor mPgpSignedUntrustedMessageColor;
    QColor mPgpSignedUntrustedTextColor;
    QColor mPgpSignedBadMessageColor;
    QColor mPgpSignedBadTextColor;
    QColor mPgpEncryptedMessageColor;
    QColor mPgpEncryptedTextColor;
};

namespace Util
{

MESSAGECORE_EXPORT QColor misspelledDefaultTextColor();
MESSAGECORE_EXPORT QColor quoteLevel1DefaultTextColor();
MESSAGECORE_EXPORT QColor quoteLevel2DefaultTextColor();
MESSAGECORE_EXPORT QColor quoteLevel3DefaultTextColor();

MESSAGECORE_EXPORT QColor pgpSignedTrustedMessageColor();
MESSAGECORE_EXPORT QColor pgpSignedTrustedTextColor();
MESSAGECORE_EXPORT QColor pgpSignedUntrustedMessageColor();
MESSAGECORE_EXPORT QColor pgpSignedUntrustedTextColor();
MESSAGECORE_EXPORT QColor pgpSignedBadMessageColor();
MESSAGECORE_EXPORT QColor pgpSignedBadTextColor();
MESSAGECORE_EXPORT QColor pgpEncryptedMessageColor();
MESSAGECORE_EXPORT QColor pgpEncryptedTextColor();

}
}

#endif // MESSAGECORE_UTIL_H
