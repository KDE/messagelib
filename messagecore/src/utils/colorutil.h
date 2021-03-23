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

