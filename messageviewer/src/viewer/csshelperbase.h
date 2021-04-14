/*  -*- c++ -*-
    csshelper.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QColor>
#include <QFont>

class QString;
class QPaintDevice;

namespace MessageViewer
{
class HeaderStylePlugin;
/**
 * @brief The CSSHelperBase class
 */
class MESSAGEVIEWER_EXPORT CSSHelperBase
{
public:
    /** Construct a CSSHelper object and set its font and color settings to
        default values.
        Sub-Classes should put their config loading here.
     */
    explicit CSSHelperBase(const QPaintDevice *pd);
    virtual ~CSSHelperBase();

    /** @return HTML head including style sheet definitions and the
        &gt;body&lt; tag */
    virtual QString htmlHead(bool fixedFont = false) const;

    /** @return The collected CSS definitions as a string */
    Q_REQUIRED_RESULT QString cssDefinitions(bool fixedFont = false) const;

    /** @return a &lt;div&gt; start tag with embedded style
        information suitable for quoted text with quote level @p level */
    Q_REQUIRED_RESULT QString quoteFontTag(int level) const;
    /** @return a &lt;div&gt; start tag with embedded style
        information suitable for non-quoted text */
    Q_REQUIRED_RESULT QString nonQuotedFontTag() const;

    Q_REQUIRED_RESULT QFont bodyFont(bool fixedFont = false, bool printing = false) const;

    void setBodyFont(const QFont &font);
    void setPrintFont(const QFont &font);

    /** @return the quote color for the given level, where level ranges from 0 to 2 **/
    Q_REQUIRED_RESULT QColor quoteColor(int level) const;
    Q_REQUIRED_RESULT QString quoteColorName(int level) const;

    Q_REQUIRED_RESULT QColor pgpWarnColor() const;

    Q_REQUIRED_RESULT QString addEndBlockQuote(int numberBlock) const;
    Q_REQUIRED_RESULT QString addStartBlockQuote(int numberBlock) const;

    Q_REQUIRED_RESULT QString extraScreenCss(const QString &headerFont) const;
    Q_REQUIRED_RESULT QString extraPrintCss(const QString &headerFont) const;
    Q_REQUIRED_RESULT QString extraCommonCss(const QString &headerFont) const;

    void setHeaderPlugin(const HeaderStylePlugin *headerPlugin);

    enum InlineMessageType { Positive, Information, Warning, Error, MESSAGE_TYPE_COUNT };

protected:
    /** Recalculate PGP frame and body colors (should be called after changing
        color settings) */
    void recalculatePGPColors();

protected:
    QFont mBodyFont;
    QFont mPrintFont;
    QFont mFixedFont;
    QFont mFixedPrintFont;
    QFont mQuoteFont;
    QColor mQuoteColor[3];
    bool mRecycleQuoteColors = false;
    bool mShrinkQuotes = false;
    bool mUseBrowserColor = false;
    QColor mForegroundColor;
    QColor mLinkColor;
    QColor mBackgroundColor;
    // colors for PGP (Frame, Header, HeaderText, Body)
    QColor cPgpOk1F;
    QColor cPgpOk1H;
    QColor cPgpOk1HT;
    QColor cPgpOk1B;
    QColor cPgpOk0F;
    QColor cPgpOk0H;
    QColor cPgpOk0HT;
    QColor cPgpOk0B;
    QColor cPgpWarnF;
    QColor cPgpWarnH;
    QColor cPgpWarnHT;
    QColor cPgpWarnB;
    QColor cPgpErrF;
    QColor cPgpErrH;
    QColor cPgpErrHT;
    QColor cPgpErrB;
    QColor cPgpEncrF;
    QColor cPgpEncrH;
    QColor cPgpEncrHT;
    QColor cPgpEncrB;

    // colors for inline message boxes, see KMessageWidget or Kirigami::InlineMessage
    QColor cInlineMessage[MESSAGE_TYPE_COUNT];

    QString imgShowUrl;
    QString imgHideUrl;

private:
    Q_REQUIRED_RESULT QString quoteCssDefinition() const;
    int fontSize(bool fixed, bool print = false) const;
    // returns CSS rules specific to the print media type
    Q_REQUIRED_RESULT QString printCssDefinitions(bool fixed) const;
    // returns CSS rules specific to the screen media type
    Q_REQUIRED_RESULT QString screenCssDefinitions(const CSSHelperBase *helper, bool fixed) const;
    // returns CSS rules common to both screen and print media types
    Q_REQUIRED_RESULT QString commonCssDefinitions() const;
    Q_REQUIRED_RESULT QString fullAddressList() const;
    Q_REQUIRED_RESULT QString linkColorDefinition() const;
    Q_REQUIRED_RESULT QString defaultScreenHeaderFont() const;
    Q_REQUIRED_RESULT QString defaultPrintHeaderFont() const;

    const QPaintDevice *mPaintDevice = nullptr;
    const HeaderStylePlugin *mHeaderPlugin = nullptr;
};
}

