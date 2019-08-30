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

namespace MessageCore {
/**
 * @brief The ColorUtil class
 */
class MESSAGECORE_EXPORT ColorUtil
{
public:
    static ColorUtil *self();

    ColorUtil();

    void updateColors();

    Q_REQUIRED_RESULT QColor misspelledDefaultTextColor() const;
    Q_REQUIRED_RESULT QColor quoteLevel1DefaultTextColor() const;
    Q_REQUIRED_RESULT QColor quoteLevel2DefaultTextColor() const;
    Q_REQUIRED_RESULT QColor quoteLevel3DefaultTextColor() const;
    Q_REQUIRED_RESULT QColor pgpSignedTrustedMessageColor() const;
    Q_REQUIRED_RESULT QColor pgpSignedTrustedTextColor() const;
    Q_REQUIRED_RESULT QColor pgpSignedUntrustedMessageColor() const;
    Q_REQUIRED_RESULT QColor pgpSignedUntrustedTextColor() const;
    Q_REQUIRED_RESULT QColor pgpSignedBadMessageColor() const;
    Q_REQUIRED_RESULT QColor pgpSignedBadTextColor() const;
    Q_REQUIRED_RESULT QColor pgpEncryptedMessageColor() const;
    Q_REQUIRED_RESULT QColor pgpEncryptedTextColor() const;
    Q_REQUIRED_RESULT QColor linkColor() const;

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
    QColor mLinkColor;
};
}

#endif // MESSAGECORE_UTIL_H
