/*
    csshelper.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003 Marc Mutz <mutz@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include "csshelperbase.h"
#include "utils/iconnamecache.h"

#include <QApplication>
#include <QPaintDevice>
#include <QPalette>
#include <QUrl>

namespace MessageViewer {
namespace {
// some QColor manipulators that hide the ugly QColor API w.r.t. HSV:
inline QColor darker(const QColor &c)
{
    int h, s, v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv(h, s, v * 4 / 5);
}

inline QColor desaturate(const QColor &c)
{
    int h, s, v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv(h, s / 8, v);
}

inline QColor fixValue(const QColor &c, int newV)
{
    int h, s, v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv(h, s, newV);
}

inline int getValueOf(const QColor &c)
{
    int h, s, v;
    c.getHsv(&h, &s, &v);
    return v;
}
}

CSSHelperBase::CSSHelperBase(const QPaintDevice *pd)
    : mRecycleQuoteColors(false)
    , mShrinkQuotes(false)
    , cHtmlWarning(QColor(0xFF, 0x40, 0x40))
    , mPaintDevice(pd)
{
    recalculatePGPColors();
    const QString imgSrcShow = QStringLiteral("quicklistOpened.png");
    const QString imgSrcHide = QStringLiteral("quicklistClosed.png");
    imgShowUrl = QUrl::fromLocalFile(MessageViewer::IconNameCache::instance()->iconPathFromLocal(imgSrcShow)).url();
    imgHideUrl = QUrl::fromLocalFile(MessageViewer::IconNameCache::instance()->iconPathFromLocal(imgSrcHide)).url();
}

CSSHelperBase::~CSSHelperBase()
{
}

void CSSHelperBase::recalculatePGPColors()
{
    // determine the frame and body color for PGP messages from the header color
    // if the header color equals the background color then the other colors are
    // also set to the background color (-> old style PGP message viewing)
    // else
    // the brightness of the frame is set to 4/5 of the brightness of the header
    // and in case of a light background color
    // the saturation of the body is set to 1/8 of the saturation of the header
    // while in case of a dark background color
    // the value of the body is set to the value of the background color

    // Check whether the user uses a light color scheme
    const int vBG = getValueOf(mBackgroundColor);
    const bool lightBG = vBG >= 128;
    if (cPgpOk1H == mBackgroundColor) {
        cPgpOk1F = mBackgroundColor;
        cPgpOk1B = mBackgroundColor;
    } else {
        cPgpOk1F = darker(cPgpOk1H);
        cPgpOk1B = lightBG ? desaturate(cPgpOk1H) : fixValue(cPgpOk1H, vBG);
    }
    if (cPgpOk0H == mBackgroundColor) {
        cPgpOk0F = mBackgroundColor;
        cPgpOk0B = mBackgroundColor;
    } else {
        cPgpOk0F = darker(cPgpOk0H);
        cPgpOk0B = lightBG ? desaturate(cPgpOk0H) : fixValue(cPgpOk0H, vBG);
    }
    if (cPgpWarnH == mBackgroundColor) {
        cPgpWarnF = mBackgroundColor;
        cPgpWarnB = mBackgroundColor;
    } else {
        cPgpWarnF = darker(cPgpWarnH);
        cPgpWarnB = lightBG ? desaturate(cPgpWarnH) : fixValue(cPgpWarnH, vBG);
    }
    if (cPgpErrH == mBackgroundColor) {
        cPgpErrF = mBackgroundColor;
        cPgpErrB = mBackgroundColor;
    } else {
        cPgpErrF = darker(cPgpErrH);
        cPgpErrB = lightBG ? desaturate(cPgpErrH) : fixValue(cPgpErrH, vBG);
    }
    if (cPgpEncrH == mBackgroundColor) {
        cPgpEncrF = mBackgroundColor;
        cPgpEncrB = mBackgroundColor;
    } else {
        cPgpEncrF = darker(cPgpEncrH);
        cPgpEncrB = lightBG ? desaturate(cPgpEncrH) : fixValue(cPgpEncrH, vBG);
    }
}

QString CSSHelperBase::addEndBlockQuote(int numberBlock) const
{
    QString blockQuote;
    for (int i = 0; i < numberBlock; ++i) {
        blockQuote += QLatin1String("</blockquote>");
    }
    return blockQuote;
}

QString CSSHelperBase::addStartBlockQuote(int numberBlock) const
{
    QString blockQuote;
    for (int i = 0; i < numberBlock; ++i) {
        blockQuote += QLatin1String("<blockquote>");
    }
    return blockQuote;
}

QString CSSHelperBase::extraScreenCss() const
{
    return {};
}

QString CSSHelperBase::extraPrintCss() const
{
    return {};
}

QString CSSHelperBase::extraCommonCss() const
{
    return {};
}

QString CSSHelperBase::cssDefinitions(bool fixed) const
{
    return
        commonCssDefinitions()
        +
        QLatin1String("@media screen {\n\n")
        +
        screenCssDefinitions(this, fixed)
        +
        QLatin1String("}\n"
                      "@media print {\n\n")
        +
        printCssDefinitions(fixed)
        +
        QLatin1String("}\n");
}

QString CSSHelperBase::htmlHead(bool fixedFont) const
{
    Q_UNUSED(fixedFont);
    return
        QStringLiteral("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
                       "<html><head><title></title></head>\n"
                       "<body>\n");
}

QString CSSHelperBase::quoteFontTag(int level) const
{
    if (level < 0) {
        level = 0;
    }
    static const int numQuoteLevels = 3;
    const int effectiveLevel = mRecycleQuoteColors
                               ? level % numQuoteLevels + 1
                               : qMin(level + 1, numQuoteLevels);
    if (level >= numQuoteLevels) {
        return QStringLiteral("<div class=\"deepquotelevel%1\">").arg(effectiveLevel);
    } else {
        return QStringLiteral("<div class=\"quotelevel%1\">").arg(effectiveLevel);
    }
}

QString CSSHelperBase::fullAddressList() const
{
    QString css = QStringLiteral("input[type=checkbox].addresslist_checkbox {display: none}\n"
                                 ".addresslist_label_short {border: 1px; border-radius: 5px; padding: 0px 10px 0px 10px; white-space: nowrap}\n"
                                 ".addresslist_label_full {border: 1px; border-radius: 5px; padding: 0px 10px 0px 10px; white-space: nowrap}\n");
    css += QStringLiteral(".addresslist_label_short {background-image:url(%1);\nbackground-repeat: no-repeat}\n").arg(imgShowUrl);
    css += QStringLiteral(".addresslist_label_full {background-image:url(%1);\nbackground-repeat: no-repeat}\n\n").arg(imgHideUrl);
    for (const QString &str : {QStringLiteral("Cc"), QStringLiteral("To"), QStringLiteral("Bcc")}) {
        css += QStringLiteral("input ~ span.fullFull%1AddressList {display: block}\n"
                              "input ~ span.shortFull%1AddressList {display: none}\n"
                              "input:checked ~ span.fullFull%1AddressList {display: none}\n"
                              "input:checked ~ span.shortFull%1AddressList {display: block}\n\n").arg(str);
    }
    return css;
}

QString CSSHelperBase::nonQuotedFontTag() const
{
    return QStringLiteral("<div class=\"noquote\">");
}

QFont CSSHelperBase::bodyFont(bool fixed, bool print) const
{
    return fixed ? (print ? mFixedPrintFont : mFixedFont)
           : (print ? mPrintFont : mBodyFont);
}

int CSSHelperBase::fontSize(bool fixed, bool print) const
{
    return bodyFont(fixed, print).pointSize();
}

namespace {
int pointsToPixel(const QPaintDevice *pd, int pointSize)
{
    return (pointSize * pd->logicalDpiY() + 36) / 72;
}
}

static const char *const quoteFontSizes[] = { "85", "80", "75" };

QString CSSHelperBase::printCssDefinitions(bool fixed) const
{
    const QString headerFont = QStringLiteral("  font-family: \"%1\" ! important;\n"
                                              "  font-size: %2pt ! important;\n")
                               .arg(mPrintFont.family())
                               .arg(mPrintFont.pointSize());
    const QPalette &pal = QApplication::palette();

    const QFont printFont = bodyFont(fixed, true /* print */);
    QString quoteCSS;
    if (printFont.italic()) {
        quoteCSS += QLatin1String("  font-style: italic ! important;\n");
    }
    if (printFont.bold()) {
        quoteCSS += QLatin1String("  font-weight: bold ! important;\n");
    }
    if (!quoteCSS.isEmpty()) {
        quoteCSS = QLatin1String("div.noquote {\n") + quoteCSS + QLatin1String("}\n\n");
    }
    quoteCSS += quoteCssDefinition();

    return
        QStringLiteral("body {\n"
                       "  font-family: \"%1\" ! important;\n"
                       "  font-size: %2pt ! important;\n"
                       "  color: #000000 ! important;\n"
                       "  background-color: #ffffff ! important\n"
                       "}\n\n")
        .arg(printFont.family(),
             QString::number(printFont.pointSize()))
        + linkColorDefinition() +
        QStringLiteral(
                       "tr.textAtmH,\n"
                       "tr.signInProgressH,\n"
                       "tr.rfc822H,\n"
                       "tr.encrH,\n"
                       "tr.signOkKeyOkH,\n"
                       "tr.signOkKeyBadH,\n"
                       "tr.signWarnH,\n"
                       "tr.signErrH,\n"
                       "div.header {\n"
                       "%1"
                       "}\n\n"

                       "div.fancy.header > div {\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "  padding: 4px ! important;\n"
                       "  border: solid %3 1px ! important;\n"
                       "  line-height: normal;\n"
                       "}\n\n"

                       "div.fancy.header > div a[href] { color: %3 ! important; }\n\n"

                       "div.fancy.header > table.outer{\n"
                       "  all: inherit;\n"
                       "  width: auto ! important;\n"
                       "  border-spacing: 0;\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "  border-bottom: solid %3 1px ! important;\n"
                       "  border-left: solid %3 1px ! important;\n"
                       "  border-right: solid %3 1px ! important;\n"
                       "}\n\n"

                       "div.spamheader {\n"
                       "  display:none ! important;\n"
                       "}\n\n"

                       "div.htmlWarn {\n"
                       "  border: 2px solid #ffffff ! important;\n"
                       "  line-height: normal;\n"
                       "}\n\n"

                       "div.senderpic{\n"
                       "  font-size:0.8em ! important;\n"
                       "  border:1px solid black ! important;\n"
                       "  background-color:%2 ! important;\n"
                       "}\n\n"

                       "div.senderstatus{\n"
                       "  text-align:center ! important;\n"
                       "}\n\n"

                       "div.noprint {\n"
                       "  display:none ! important;\n"
                       "}\n\n"
                       )
        .arg(headerFont,
             pal.color(QPalette::Background).name(),
             pal.color(QPalette::Foreground).name())
        + quoteCSS + fullAddressList();
}

QString CSSHelperBase::linkColorDefinition() const
{
    const QString linkColor = mLinkColor.name();
    return QStringLiteral("a {\n"
                   "  color: %1 ! important;\n"
                   "  text-decoration: none ! important;\n"
                   "}\n\n").arg(linkColor);
}

QString CSSHelperBase::quoteCssDefinition() const
{
    QString quoteCSS;
    QString blockQuote;
    for (int i = 0; i < 9; ++i) {
        blockQuote += QLatin1String("blockquote ");
        quoteCSS += QString::fromLatin1("%2{\n"
                                        "  margin: 4pt 0 4pt 0;\n"
                                        "  padding: 0 0 0 1em;\n"
                                        "  border-left: 2px solid %1;\n"
                                        "  unicode-bidi: -webkit-plaintext\n"
                                        "}\n\n").arg(quoteColorName(i)).arg(blockQuote);
    }
    quoteCSS += QLatin1String(".quotemarks{\n"
                              "  color:transparent;\n"
                              "  font-size:0px;\n"
                              "}\n\n");
    quoteCSS += QLatin1String(".quotemarksemptyline{\n"
                              "  color:transparent;\n"
                              "  font-size:0px;\n"
                              "  line-height: 12pt;\n"
                              "}\n\n");
    return quoteCSS;
}

QString CSSHelperBase::screenCssDefinitions(const CSSHelperBase *helper, bool fixed) const
{
    const QString fgColor = mForegroundColor.name();
    const QString bgColor = mBackgroundColor.name();
    const QString headerFont = QStringLiteral("  font-family: \"%1\" ! important;\n"
                                              "  font-size: %2px ! important;\n")
                               .arg(mBodyFont.family())
                               .arg(pointsToPixel(helper->mPaintDevice, mBodyFont.pointSize()));
    const QString background = QStringLiteral("  background-color: %1 ! important;\n").arg(bgColor);
    const QString bodyFontSize = QString::number(pointsToPixel(helper->mPaintDevice, fontSize(
                                                                   fixed))) + QLatin1String("px");
    const QPalette &pal = QApplication::palette();

    QString quoteCSS;
    if (bodyFont(fixed).italic()) {
        quoteCSS += QLatin1String("  font-style: italic ! important;\n");
    }
    if (bodyFont(fixed).bold()) {
        quoteCSS += QLatin1String("  font-weight: bold ! important;\n");
    }
    if (!quoteCSS.isEmpty()) {
        quoteCSS = QLatin1String("div.noquote {\n") + quoteCSS + QLatin1String("}\n\n");
    }

    // CSS definitions for quote levels 1-3
    for (int i = 0; i < 3; ++i) {
        quoteCSS += QStringLiteral("div.quotelevel%1 {\n"
                                   "  color: %2 ! important;\n")
                    .arg(QString::number(i + 1), quoteColorName(i));
        if (mQuoteFont.italic()) {
            quoteCSS += QLatin1String("  font-style: italic ! important;\n");
        }
        if (mQuoteFont.bold()) {
            quoteCSS += QLatin1String("  font-weight: bold ! important;\n");
        }
        if (mShrinkQuotes) {
            quoteCSS += QLatin1String("  font-size: ") + QString::fromLatin1(quoteFontSizes[i])
                        + QLatin1String("% ! important;\n");
        }
        quoteCSS += QLatin1String("}\n\n");
    }

    // CSS definitions for quote levels 4+
    for (int i = 0; i < 3; ++i) {
        quoteCSS += QStringLiteral("div.deepquotelevel%1 {\n"
                                   "  color: %2 ! important;\n")
                    .arg(QString::number(i + 1), quoteColorName(i));
        if (mQuoteFont.italic()) {
            quoteCSS += QLatin1String("  font-style: italic ! important;\n");
        }
        if (mQuoteFont.bold()) {
            quoteCSS += QLatin1String("  font-weight: bold ! important;\n");
        }
        if (mShrinkQuotes) {
            quoteCSS += QLatin1String("  font-size: 70% ! important;\n");
        }
        quoteCSS += QLatin1String("}\n\n");
    }

    quoteCSS += quoteCssDefinition();
    return
        QStringLiteral("body {\n"
                       "  font-family: \"%1\" ! important;\n"
                       "  font-size: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "%4"
                       "}\n\n")
        .arg(bodyFont(fixed).family(),
             bodyFontSize,
             fgColor,
             background)
        + linkColorDefinition()
            +
        QStringLiteral("a.white {\n"
                       "  color: white ! important;\n"
                       "}\n\n"

                       "a.black {\n"
                       "  color: black ! important;\n"
                       "}\n\n"

                       "table.textAtm { background-color: %1 ! important; }\n\n"

                       "tr.textAtmH {\n"
                       "  background-color: %2 ! important;\n"
                       "%3"
                       "}\n\n"

                       "tr.textAtmB {\n"
                       "  background-color: %2 ! important;\n"
                       "}\n\n"

                       "table.signInProgress,\n"
                       "table.rfc822 {\n"
                       "  background-color: %2 ! important;\n"
                       "}\n\n"

                       "tr.signInProgressH,\n"
                       "tr.rfc822H {\n"
                       "%3"
                       "}\n\n").arg(fgColor, bgColor, headerFont)
        + QStringLiteral("table.encr {\n"
                       "  background-color: %1 ! important;\n"
                       "}\n\n"

                       "tr.encrH {\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "%4"
                       "}\n\n"

                       "tr.encrB { background-color: %5 ! important; }\n\n")
        .arg(cPgpEncrF.name(),
             cPgpEncrH.name(),
             cPgpEncrHT.name(),
             headerFont,
             cPgpEncrB.name())
        + QStringLiteral("table.signOkKeyOk {\n"
                       "  background-color: %1 ! important;\n"
                       "}\n\n"

                       "tr.signOkKeyOkH {\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "%4"
                       "}\n\n"

                       "tr.signOkKeyOkB { background-color: %5 ! important; }\n\n")
        .arg(cPgpOk1F.name(),
             cPgpOk1H.name(),
             cPgpOk1HT.name(),
             headerFont,
             cPgpOk1B.name())
        + QStringLiteral("table.signOkKeyBad {\n"
                       "  background-color: %1 ! important;\n"
                       "}\n\n"

                       "tr.signOkKeyBadH {\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "%4"
                       "}\n\n"

                       "tr.signOkKeyBadB { background-color: %5 ! important; }\n\n")
        .arg(cPgpOk0F.name(),
             cPgpOk0H.name(),
             cPgpOk0HT.name(),
             headerFont,
             cPgpOk0B.name())
        + QStringLiteral("table.signWarn {\n"
                       "  background-color: %1 ! important;\n"
                       "}\n\n"

                       "tr.signWarnH {\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "%4"
                       "}\n\n"

                       "tr.signWarnB { background-color: %5 ! important; }\n\n")
        .arg(cPgpWarnF.name(),
             cPgpWarnH.name(),
             cPgpWarnHT.name(),
             headerFont,
             cPgpWarnB.name())
        +
        QStringLiteral("table.signErr {\n"
                       "  background-color: %1 ! important;\n"
                       "}\n\n"

                       "tr.signErrH {\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "%4"
                       "}\n\n"

                       "tr.signErrB { background-color: %5 ! important; }\n\n")
        .arg(cPgpErrF.name(),
             cPgpErrH.name(),
             cPgpErrHT.name(),
             headerFont,
             cPgpErrB.name())
        +
        QStringLiteral("div.htmlWarn {\n"
                       "  border: 2px solid %1 ! important;\n"
                       "  line-height: normal;\n"
                       "}\n\n")
        .arg(cHtmlWarning.name())
        +
        QStringLiteral("div.header {\n"
                       "%1"
                       "}\n\n"

                       "div.fancy.header > div {\n"
                       "  background-color: %2 ! important;\n"
                       "  color: %3 ! important;\n"
                       "  border: solid %4 1px ! important;\n"
                       "  line-height: normal;\n"
                       "}\n\n"

                       "div.fancy.header > div a[href] { color: %3 ! important; }\n\n"

                       "div.fancy.header > div a[href]:hover { text-decoration: underline ! important; }\n\n"

                       "div.fancy.header > div.spamheader {\n"
                       "  background-color: #cdcdcd ! important;\n"
                       "  border-top: 0px ! important;\n"
                       "  padding: 3px ! important;\n"
                       "  color: black ! important;\n"
                       "  font-weight: bold ! important;\n"
                       "  font-size: smaller ! important;\n"
                       "}\n\n"

                       "div.fancy.header > table.outer {\n"
                       "  all: inherit;\n"
                       "  width: auto ! important;\n"
                       "  border-spacing: 0;\n"
                       "  background-color: %5 ! important;\n"
                       "  color: %4 ! important;\n"
                       "  border-bottom: solid %4 1px ! important;\n"
                       "  border-left: solid %4 1px ! important;\n"
                       "  border-right: solid %4 1px ! important;\n"
                       "}\n\n"

                       "div.senderpic{\n"
                       "  padding: 0px ! important;\n"
                       "  font-size:0.8em ! important;\n"
                       "  border:1px solid %6 ! important;\n"
                       "  background-color:%5 ! important;\n"
                       "}\n\n"

                       "div.senderstatus{\n"
                       "  text-align:center ! important;\n"
                       "}\n\n"
                       )

        .arg(headerFont)
        .arg(pal.color(QPalette::Highlight).name(),
             pal.color(QPalette::HighlightedText).name(),
             pal.color(QPalette::Foreground).name(),
             pal.color(QPalette::Background).name())
        .arg(pal.color(QPalette::Mid).name())
        + quoteCSS + fullAddressList();
}

QString CSSHelperBase::commonCssDefinitions() const
{
    const QPalette &pal = QApplication::palette();
    const QString headerFont = QStringLiteral("font-family: \"%1\" ! important;\n"
                                              "  font-size: %2px ! important;\n")
                               .arg(mBodyFont.family())
                               .arg(pointsToPixel(this->mPaintDevice, mBodyFont.pointSize()));

    return
        QStringLiteral("div.header {\n"
                       "  margin-bottom: 10pt ! important;\n"
                       "}\n\n"

                       "table.textAtm {\n"
                       "  margin-top: 10pt ! important;\n"
                       "  margin-bottom: 10pt ! important;\n"
                       "}\n\n"

                       "tr.textAtmH,\n"
                       "tr.textAtmB,\n"
                       "tr.rfc822B {\n"
                       "  font-weight: normal ! important;\n"
                       "}\n\n"

                       "tr.signInProgressH,\n"
                       "tr.rfc822H,\n"
                       "tr.encrH,\n"
                       "tr.signOkKeyOkH,\n"
                       "tr.signOkKeyBadH,\n"
                       "tr.signWarnH,\n"
                       "tr.signErrH {\n"
                       "  font-weight: bold ! important;\n"
                       "}\n\n"

                       "tr.textAtmH td,\n"
                       "tr.textAtmB td {\n"
                       "  padding: 3px ! important;\n"
                       "}\n\n"

                       "table.rfc822 {\n"
                       "  width: 100% ! important;\n"
                       "  border: solid 1px black ! important;\n"
                       "  margin-top: 10pt ! important;\n"
                       "  margin-bottom: 10pt ! important;\n"
                       "}\n\n"

                       "table.textAtm,\n"
                       "table.encr,\n"
                       "table.signWarn,\n"
                       "table.signErr,\n"
                       "table.signOkKeyBad,\n"
                       "table.signOkKeyOk,\n"
                       "table.signInProgress,\n"


                       "div.fancy.header table {\n"
                       "  width: 100% ! important;\n"
                       "  border-width: 0px ! important;\n"
                       "  line-height: normal;\n"
                       "}\n\n"

                       "div.fancy.header > div {\n"
                       "  font-weight: bold ! important;\n"
                       "  padding: 4px ! important;\n"
                       "  line-height: normal;\n"
                       "}\n\n"

                       "div.fancy.header table {\n"
                       "  padding: 2px ! important;\n"
                       "  text-align: left ! important;\n"
                       "  border-collapse: separate ! important;\n"
                       "}\n\n"

                       "div.fancy.header table th {\n"
                       "  %3\n"
                       "  padding: 0px ! important;\n"
                       "  white-space: nowrap ! important;\n"
                       "  border-spacing: 0px ! important;\n"
                       "  text-align: left ! important;\n"
                       "  vertical-align: top ! important;\n"
                       "  background-color: %1 ! important;\n"
                       "  color: %2 ! important;\n"
                       "  border: 1px ! important;\n"

                       "}\n\n"

                       "div.fancy.header table td {\n"
                       "  %3\n"
                       "  padding: 0px ! important;\n"
                       "  border-spacing: 0px ! important;\n"
                       "  text-align: left ! important;\n"
                       "  vertical-align: top ! important;\n"
                       "  width: 100% ! important;\n"
                       "  background-color: %1 ! important;\n"
                       "  color: %2 ! important;\n"
                       "  border: 1px ! important;\n"
                       "}\n\n"

                       "div.fancy.header table a:hover {\n"
                       "  background-color: transparent ! important;\n"
                       "}\n\n"

                       "div.htmlWarn {\n"
                       "  margin: 0px 5% ! important;\n"
                       "  padding: 10px ! important;\n"
                       "  text-align: left ! important;\n"
                       "  line-height: normal;\n"
                       "}\n\n"


                       "div.quotelevelmark {\n"
                       "  position: absolute;\n"
                       "  margin-left:-10px;\n"
                       "}\n\n").arg(pal.color(QPalette::Background).name()).arg(pal.color(QPalette::Foreground).name()).arg(headerFont);
}

void CSSHelperBase::setBodyFont(const QFont &font)
{
    mBodyFont = font;
}

void CSSHelperBase::setPrintFont(const QFont &font)
{
    mPrintFont = font;
}

QString CSSHelperBase::quoteColorName(int level) const
{
    return quoteColor(level).name();
}

QColor CSSHelperBase::quoteColor(int level) const
{
    const int actualLevel = qMax(level, 0) % 3;
    return mQuoteColor[actualLevel];
}

QColor CSSHelperBase::pgpWarnColor() const
{
    return cPgpWarnH;
}
}
