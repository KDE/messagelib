/*
 * SPDX-FileCopyrightText: 2015 Daniel Vrátil <dvratil@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#pragma once

#include "messagecore_export.h"

#include <QColor>

namespace MessageCore
{
/**
 * @brief The ColorUtil class
 */
class MESSAGECORE_EXPORT ColorUtil
{
public:
    static ColorUtil *self();

    ColorUtil();

    void updateColors();

    [[nodiscard]] QColor misspelledDefaultTextColor() const;
    [[nodiscard]] QColor quoteLevel1DefaultTextColor() const;
    [[nodiscard]] QColor quoteLevel2DefaultTextColor() const;
    [[nodiscard]] QColor quoteLevel3DefaultTextColor() const;
    [[nodiscard]] QColor pgpSignedTrustedMessageColor() const;
    [[nodiscard]] QColor pgpSignedTrustedTextColor() const;
    [[nodiscard]] QColor pgpSignedUntrustedMessageColor() const;
    [[nodiscard]] QColor pgpSignedUntrustedTextColor() const;
    [[nodiscard]] QColor pgpSignedBadMessageColor() const;
    [[nodiscard]] QColor pgpSignedBadTextColor() const;
    [[nodiscard]] QColor pgpEncryptedMessageColor() const;
    [[nodiscard]] QColor pgpEncryptedTextColor() const;
    [[nodiscard]] QColor linkColor() const;

private:
    MESSAGECORE_NO_EXPORT void initializeColors();
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
