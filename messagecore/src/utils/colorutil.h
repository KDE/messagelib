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
/*!
 * \class MessageCore::ColorUtil
 * \inmodule MessageCore
 * \inheaderfile MessageCore/ColorUtil
 * \brief The ColorUtil class
 */
class MESSAGECORE_EXPORT ColorUtil
{
public:
    /*!
     * Returns the singleton instance of ColorUtil.
     */
    static ColorUtil *self();

    /*!
     * Creates a new ColorUtil instance.
     */
    ColorUtil();

    /*!
     * Updates the colors based on current settings.
     */
    void updateColors();

    /*!
     * Returns the default text color for misspelled words.
     */
    [[nodiscard]] QColor misspelledDefaultTextColor() const;
    /*!
     * Returns the text color for PGP quote level 1.
     */
    [[nodiscard]] QColor quoteLevel1DefaultTextColor() const;
    /*!
     * Returns the text color for PGP quote level 2.
     */
    [[nodiscard]] QColor quoteLevel2DefaultTextColor() const;
    /*!
     * Returns the text color for PGP quote level 3.
     */
    [[nodiscard]] QColor quoteLevel3DefaultTextColor() const;
    /*!
     * Returns the message background color for trusted PGP signed messages.
     */
    [[nodiscard]] QColor pgpSignedTrustedMessageColor() const;
    /*!
     * Returns the text color for trusted PGP signed messages.
     */
    [[nodiscard]] QColor pgpSignedTrustedTextColor() const;
    /*!
     * Returns the message background color for untrusted PGP signed messages.
     */
    [[nodiscard]] QColor pgpSignedUntrustedMessageColor() const;
    /*!
     * Returns the text color for untrusted PGP signed messages.
     */
    [[nodiscard]] QColor pgpSignedUntrustedTextColor() const;
    /*!
     * Returns the message background color for bad PGP signed messages.
     */
    [[nodiscard]] QColor pgpSignedBadMessageColor() const;
    /*!
     * Returns the text color for bad PGP signed messages.
     */
    [[nodiscard]] QColor pgpSignedBadTextColor() const;
    /*!
     * Returns the message background color for PGP encrypted messages.
     */
    [[nodiscard]] QColor pgpEncryptedMessageColor() const;
    /*!
     * Returns the text color for PGP encrypted messages.
     */
    [[nodiscard]] QColor pgpEncryptedTextColor() const;
    /*!
     * Returns the color for links.
     */
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
