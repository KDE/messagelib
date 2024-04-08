/*
    csshelper.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "csshelperbase.h"
#include "header/headerstyleplugin.h"
#include "utils/iconnamecache.h"

#include <QApplication>
#include <QPaintDevice>
#include <QPalette>
#include <QUrl>

namespace MessageViewer
{
namespace
{
// some QColor manipulators that hide the ugly QColor API w.r.t. HSV:
inline QColor darker(const QColor &c)
{
    int h;
    int s;
    int v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv(h, s, v * 4 / 5);
}

inline QColor desaturate(const QColor &c)
{
    int h;
    int s;
    int v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv(h, s / 8, v);
}

inline QColor fixValue(const QColor &c, int newV)
{
    int h;
    int s;
    int v;
    c.getHsv(&h, &s, &v);
    return QColor::fromHsv(h, s, newV);
}

inline int getValueOf(const QColor &c)
{
    int h;
    int s;
    int v;
    c.getHsv(&h, &s, &v);
    return v;
}

static const struct {
    CSSHelperBase::InlineMessageType type;
    const char *cssName;
} inlineMessageStyles[] = {{CSSHelperBase::Positive, "inlineMessagePositive"},
                           {CSSHelperBase::Information, "inlineMessageInformation"},
                           {CSSHelperBase::Warning, "inlineMessageWarning"},
                           {CSSHelperBase::Error, "inlineMessageError"}};
}

CSSHelperBase::CSSHelperBase(const QPaintDevice *pd)
    : mPaintDevice(pd)
{
    const QString imgSrcShow = QStringLiteral("quicklistClosed.png");
    const QString imgSrcHide = QStringLiteral("quicklistOpened.png");
    imgShowUrl = QUrl::fromLocalFile(MessageViewer::IconNameCache::instance()->iconPathFromLocal(imgSrcShow)).url();
    imgHideUrl = QUrl::fromLocalFile(MessageViewer::IconNameCache::instance()->iconPathFromLocal(imgSrcHide)).url();
}

CSSHelperBase::~CSSHelperBase() = default;

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
        blockQuote += QLatin1StringView("</blockquote>");
    }
    return blockQuote;
}

QString CSSHelperBase::addStartBlockQuote(int numberBlock) const
{
    QString blockQuote;
    for (int i = 0; i < numberBlock; ++i) {
        blockQuote += QLatin1StringView("<blockquote>");
    }
    return blockQuote;
}

QString CSSHelperBase::extraScreenCss(const QString &headerFont) const
{
    if (mHeaderPlugin) {
        return mHeaderPlugin->extraScreenCss(headerFont);
    }
    return {};
}

QString CSSHelperBase::extraPrintCss(const QString &headerFont) const
{
    if (mHeaderPlugin) {
        return mHeaderPlugin->extraPrintCss(headerFont);
    }
    return {};
}

QString CSSHelperBase::extraCommonCss(const QString &headerFont) const
{
    QString result;
    if (mHeaderPlugin) {
        result = mHeaderPlugin->extraCommonCss(headerFont);
    }
    if (result.isEmpty()) {
        // Add default value
        result = QStringLiteral(
            "div.header table {\n"
            "  width: 100% ! important;\n"
            "  border-width: 0px ! important;\n"
            "  line-height: normal;\n"
            "}\n\n");
    }
    return result;
}

QString CSSHelperBase::cssDefinitions(const HtmlHeadSettings &htmlHeadSetting) const
{
    return commonCssDefinitions() + QLatin1StringView("@media screen {\n\n") + screenCssDefinitions(this, htmlHeadSetting)
        + QLatin1StringView(
               "}\n"
               "@media print {\n\n")
        + printCssDefinitions(htmlHeadSetting) + QLatin1StringView("}\n");
}

QString CSSHelperBase::htmlHead(const HtmlHeadSettings &htmlHeadSettings) const
{
    Q_UNUSED(htmlHeadSettings)
    return QStringLiteral(
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
        "<html><head><title></title></head>\n"
        "<body>\n");
}

QString CSSHelperBase::quoteFontTag(int level) const
{
    if (level < 0) {
        level = 0;
    }
    static const int numQuoteLevels = 3;
    const int effectiveLevel = mRecycleQuoteColors ? level % numQuoteLevels + 1 : qMin(level + 1, numQuoteLevels);
    if (level >= numQuoteLevels) {
        return QStringLiteral("<div class=\"deepquotelevel%1\">").arg(effectiveLevel);
    } else {
        return QStringLiteral("<div class=\"quotelevel%1\">").arg(effectiveLevel);
    }
}

QString CSSHelperBase::fullAddressList() const
{
    QString css = QStringLiteral(
        "input[type=checkbox].addresslist_checkbox {display: none}\n"
        ".addresslist_label_short {border: 1px; border-radius: 5px; padding: 0px 10px 0px 10px; white-space: nowrap}\n"
        ".addresslist_label_full {border: 1px; border-radius: 5px; padding: 0px 10px 0px 10px; white-space: nowrap}\n");
    css += QStringLiteral(".addresslist_label_short {background-image:url(%1);\nbackground-repeat: no-repeat}\n").arg(imgShowUrl);
    css += QStringLiteral(".addresslist_label_full {background-image:url(%1);\nbackground-repeat: no-repeat}\n\n").arg(imgHideUrl);
    for (const QString &str : {QStringLiteral("Cc"), QStringLiteral("To"), QStringLiteral("Bcc")}) {
        css += QStringLiteral(
                   "input ~ span.fullFull%1AddressList {display: block}\n"
                   "input ~ span.shortFull%1AddressList {display: none}\n"
                   "input:checked ~ span.fullFull%1AddressList {display: none}\n"
                   "input:checked ~ span.shortFull%1AddressList {display: block}\n\n")
                   .arg(str);
    }
    return css;
}

QString CSSHelperBase::nonQuotedFontTag() const
{
    return QStringLiteral("<div class=\"noquote\">");
}

QFont CSSHelperBase::bodyFont(bool fixed, bool print) const
{
    return fixed ? (print ? mFixedPrintFont : mFixedFont) : (print ? mPrintFont : mBodyFont);
}

int CSSHelperBase::fontSize(bool fixed, bool print) const
{
    return bodyFont(fixed, print).pointSize();
}

namespace
{
int pointsToPixel(const QPaintDevice *pd, int pointSize)
{
    return (pointSize * pd->logicalDpiY() + 36) / 72;
}
}

void CSSHelperBase::setHeaderPlugin(const HeaderStylePlugin *headerPlugin)
{
    mHeaderPlugin = headerPlugin;
}

static const char *const quoteFontSizes[] = {"85", "80", "75"};

QString CSSHelperBase::printCssDefinitions(const HtmlHeadSettings &htmlHeadSettings) const
{
    const QString headerFont = defaultPrintHeaderFont();

    const QFont printFont = bodyFont(htmlHeadSettings.fixedFont, true /* print */);
    QString quoteCSS;
    if (printFont.italic()) {
        quoteCSS += QLatin1StringView("  font-style: italic ! important;\n");
    }
    if (printFont.bold()) {
        quoteCSS += QLatin1StringView("  font-weight: bold ! important;\n");
    }
    if (!quoteCSS.isEmpty()) {
        quoteCSS = QLatin1StringView("div.noquote {\n") + quoteCSS + QLatin1StringView("}\n\n");
    }
    quoteCSS += quoteCssDefinition();

    QStringList inlineMessageCss;
    inlineMessageCss.reserve(MESSAGE_TYPE_COUNT);
    for (const auto &msgStyle : inlineMessageStyles) {
        inlineMessageCss.push_back(QLatin1StringView("div.") + QString::fromLatin1(msgStyle.cssName));
    }

    return QStringLiteral(
               "body {\n"
               "  font-family: \"%1\" ! important;\n"
               "  font-size: %2pt ! important;\n"
               "  color: #000000 ! important;\n"
               "  background-color: #ffffff ! important\n"
               "}\n\n")
               .arg(printFont.family(), QString::number(printFont.pointSize()))
        + linkColorDefinition(htmlHeadSettings)
        + QStringLiteral(
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

              "%2"

              "div.spamheader {\n"
              "  display:none ! important;\n"
              "}\n\n"

              "%3 {\n"
              "  display:none ! important;\n"
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
              "}\n\n")
              .arg(headerFont, extraPrintCss(headerFont), inlineMessageCss.join(QLatin1StringView(", ")))
        + quoteCSS + fullAddressList();
}

QString CSSHelperBase::linkColorDefinition(const HtmlHeadSettings &htmlHeadSettings) const
{
    const QString linkColor = mLinkColor.name();
    if (useBrowserColor(htmlHeadSettings)) {
        const QString bgColor = mBackgroundColor.name();
        const QString background = QStringLiteral("  background: %1 ! important;\n").arg(bgColor);

        return QStringLiteral(
                   "div#headerbox a:link {\n"
                   "  color: %1 ! important;\n"
                   "  text-decoration: none ! important;\n"
                   "}\n\n"
                   "div#header a:link {\n"
                   "  color: %1 ! important;\n"
                   "  text-decoration: none ! important;\n"
                   "}\n\n"
                   "div.header {\n"
                   "    %2"
                   "}\n\n"
                   "div#headerbox {\n"
                   "    %2"
                   "}\n\n")
            .arg(linkColor, background);
    } else {
        return QStringLiteral(
                   "a {\n"
                   "  color: %1 ! important;\n"
                   "  text-decoration: none ! important;\n"
                   "}\n\n")
            .arg(linkColor);
    }
}

QString CSSHelperBase::quoteCssDefinition() const
{
    QString quoteCSS;
    QString blockQuote;
    for (int i = 0; i < 9; ++i) {
        blockQuote += QStringLiteral("blockquote ");
        quoteCSS += QStringLiteral(
                        "%2{\n"
                        "  margin: 4pt 0 4pt 0;\n"
                        "  padding: 0 0 0 1em;\n"
                        "  border-left: 2px solid %1;\n"
                        "  unicode-bidi: plaintext\n"
                        "}\n\n")
                        .arg(quoteColorName(i), blockQuote);
    }
    quoteCSS += QStringLiteral(
        ".quotemarks{\n"
        "  color:transparent;\n"
        "  font-size:0px;\n"
        "}\n\n");
    quoteCSS += QStringLiteral(
        ".quotemarksemptyline{\n"
        "  color:transparent;\n"
        "  font-size:0px;\n"
        "  line-height: 12pt;\n"
        "}\n\n");
    return quoteCSS;
}

QString CSSHelperBase::defaultPrintHeaderFont() const
{
    const QString headerFont = QStringLiteral(
                                   "  font-family: \"%1\" ! important;\n"
                                   "  font-size: %2pt ! important;\n")
                                   .arg(mPrintFont.family())
                                   .arg(mPrintFont.pointSize());
    return headerFont;
}

QString CSSHelperBase::defaultScreenHeaderFont() const
{
    const QString headerFont = QStringLiteral(
                                   "  font-family: \"%1\" ! important;\n"
                                   "  font-size: %2px ! important;\n")
                                   .arg(mBodyFont.family())
                                   .arg(pointsToPixel(mPaintDevice, mBodyFont.pointSize()));
    return headerFont;
}

bool CSSHelperBase::useBrowserColor(const HtmlHeadSettings &htmlHeadSettings) const
{
    return mUseBrowserColor && htmlHeadSettings.htmlFormat;
}

const HeaderStylePlugin *CSSHelperBase::headerPlugin() const
{
    return mHeaderPlugin;
}

QString CSSHelperBase::screenCssDefinitions(const CSSHelperBase *helper, const HtmlHeadSettings &htmlHeadSettings) const
{
    const QString bgColor = mBackgroundColor.name();
    const QString headerFont = defaultScreenHeaderFont();
    const QString fgColor = useBrowserColor(htmlHeadSettings) ? QStringLiteral("black") : mForegroundColor.name();
    const QString background = useBrowserColor(htmlHeadSettings) ? QString() : QStringLiteral("  background-color: %1 ! important;\n").arg(bgColor);
    const QString signWarnBColorName = useBrowserColor(htmlHeadSettings) ? QStringLiteral("white") : cPgpWarnB.name();
    const QString cPgpErrBColorName = useBrowserColor(htmlHeadSettings) ? QStringLiteral("white") : cPgpErrB.name();
    const QString cPgpEncrBColorName = useBrowserColor(htmlHeadSettings) ? QStringLiteral("white") : cPgpEncrB.name();
    const QString cPgpOk1BColorName = useBrowserColor(htmlHeadSettings) ? QStringLiteral("white") : cPgpOk1B.name();
    const QString cPgpOk0BColorName = useBrowserColor(htmlHeadSettings) ? QStringLiteral("white") : cPgpOk0B.name();
    const QString bodyFontSize = QString::number(pointsToPixel(helper->mPaintDevice, fontSize(htmlHeadSettings.fixedFont))) + QLatin1StringView("px");
    const QPalette &pal = QApplication::palette();

    QString quoteCSS;
    if (bodyFont(htmlHeadSettings.fixedFont).italic()) {
        quoteCSS += QLatin1StringView("  font-style: italic ! important;\n");
    }
    if (bodyFont(htmlHeadSettings.fixedFont).bold()) {
        quoteCSS += QLatin1StringView("  font-weight: bold ! important;\n");
    }
    if (!quoteCSS.isEmpty()) {
        quoteCSS = QLatin1StringView("div.noquote {\n") + quoteCSS + QLatin1StringView("}\n\n");
    }

    // CSS definitions for quote levels 1-3
    for (int i = 0; i < 3; ++i) {
        quoteCSS += QStringLiteral(
                        "div.quotelevel%1 {\n"
                        "  color: %2 ! important;\n")
                        .arg(QString::number(i + 1), quoteColorName(i));
        if (mQuoteFont.italic()) {
            quoteCSS += QStringLiteral("  font-style: italic ! important;\n");
        }
        if (mQuoteFont.bold()) {
            quoteCSS += QStringLiteral("  font-weight: bold ! important;\n");
        }
        if (mShrinkQuotes) {
            quoteCSS += QLatin1StringView("  font-size: ") + QString::fromLatin1(quoteFontSizes[i]) + QLatin1StringView("% ! important;\n");
        }
        quoteCSS += QStringLiteral("}\n\n");
    }

    // CSS definitions for quote levels 4+
    for (int i = 0; i < 3; ++i) {
        quoteCSS += QStringLiteral(
                        "div.deepquotelevel%1 {\n"
                        "  color: %2 ! important;\n")
                        .arg(QString::number(i + 1), quoteColorName(i));
        if (mQuoteFont.italic()) {
            quoteCSS += QStringLiteral("  font-style: italic ! important;\n");
        }
        if (mQuoteFont.bold()) {
            quoteCSS += QStringLiteral("  font-weight: bold ! important;\n");
        }
        if (mShrinkQuotes) {
            quoteCSS += QStringLiteral("  font-size: 70% ! important;\n");
        }
        quoteCSS += QLatin1StringView("}\n\n");
    }

    quoteCSS += quoteCssDefinition();

    // CSS definitions for inline message boxes
    QString inlineMessageCss;
    for (const auto &msgStyle : inlineMessageStyles) {
        const auto c = cInlineMessage[msgStyle.type];
        inlineMessageCss += QStringLiteral(
                                R"(
            div.%1 {
                border: 1px solid rgba(%2, %3, %4) ! important;
                border-radius: 2px;
                box-shadow: 0px 0px 6px rgba(0, 0, 0, 0.5);
                background-color: rgba(%2, %3, %4, 0.2) ! important;
            }
            div.%1 a:link {
                color: %5 ! important;
                text-decoration: none ! important;
            }
         )")
                                .arg(QString::fromLatin1(msgStyle.cssName))
                                .arg(c.red())
                                .arg(c.green())
                                .arg(c.blue())
                                .arg(mLinkColor.name());
    }

    const auto scrollBarCss = QStringLiteral(
        "html::-webkit-scrollbar {\n"
        "  /* we'll add padding as \"border\" in the thumb*/\n"
        "    height: 20px;\n"
        "    width: 20px;\n"
        "    background: white;\n"
        "    border-left: 1px solid #e5e5e5;\n"
        "    padding-left: 1px;\n"
        "}\n\n"

        "html::-webkit-scrollbar-track {\n"
        "    border-radius: 20px;\n"
        "    width: 6px !important;\n"
        "    box-sizing: content-box;\n"
        "}\n\n"

        "html::-webkit-scrollbar-thumb {\n"
        "    background-color: #CBCDCD;\n"
        "    border: 6px solid transparent;\n"
        "    border-radius: 20px;\n"
        "    background-clip: content-box;\n"
        "    width: 8px !important; /* 20px scrollbar - 2 * 6px border */\n"
        "    box-sizing: content-box;\n"
        "    min-height: 30px;\n"
        "}\n\n"

        "html::-webkit-scrollbar-thumb:window-inactive {\n"
        "   background-color: #949699; /* when window is inactive it's gray */\n"
        "}\n\n"

        "html::-webkit-scrollbar-thumb:hover {\n"
        "    background-color: #93CEE9; /* hovered is a lighter blue */\n"
        "}\n\n"

        "html::-webkit-scrollbar-corner {\n"
        "    background-color: white;\n"
        "}\n\n");

    return QStringLiteral(
               "body {\n"
               "  font-family: \"%1\" ! important;\n"
               "  font-size: %2 ! important;\n"
               "  color: %3 ! important;\n"
               "%4"
               "}\n\n")
               .arg(bodyFont(htmlHeadSettings.fixedFont).family(), bodyFontSize, fgColor, background)
        + linkColorDefinition(htmlHeadSettings)
        + QStringLiteral(
              "a.white {\n"
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
              "}\n\n")
              .arg(fgColor, bgColor, headerFont)
        + QStringLiteral(
              "table.encr {\n"
              "  background-color: %1 ! important;\n"
              "}\n\n"

              "tr.encrH {\n"
              "  background-color: %2 ! important;\n"
              "  color: %3 ! important;\n"
              "%4"
              "}\n\n"

              "tr.encrB { background-color: %5 ! important; }\n\n")
              .arg(cPgpEncrF.name(), cPgpEncrH.name(), cPgpEncrHT.name(), headerFont, cPgpEncrBColorName)
        + QStringLiteral(
              "table.signOkKeyOk {\n"
              "  background-color: %1 ! important;\n"
              "}\n\n"

              "tr.signOkKeyOkH {\n"
              "  background-color: %2 ! important;\n"
              "  color: %3 ! important;\n"
              "%4"
              "}\n\n"

              "tr.signOkKeyOkB { background-color: %5 ! important; }\n\n")
              .arg(cPgpOk1F.name(), cPgpOk1H.name(), cPgpOk1HT.name(), headerFont, cPgpOk1BColorName)
        + QStringLiteral(
              "table.signOkKeyBad {\n"
              "  background-color: %1 ! important;\n"
              "}\n\n"

              "tr.signOkKeyBadH {\n"
              "  background-color: %2 ! important;\n"
              "  color: %3 ! important;\n"
              "%4"
              "}\n\n"

              "tr.signOkKeyBadB { background-color: %5 ! important; }\n\n")
              .arg(cPgpOk0F.name(), cPgpOk0H.name(), cPgpOk0HT.name(), headerFont, cPgpOk0BColorName)
        + QStringLiteral(
              "table.signWarn {\n"
              "  background-color: %1 ! important;\n"
              "}\n\n"

              "tr.signWarnH {\n"
              "  background-color: %2 ! important;\n"
              "  color: %3 ! important;\n"
              "%4"
              "}\n\n"

              "tr.signWarnB { background-color: %5 ! important; }\n\n")
              .arg(cPgpWarnF.name(), cPgpWarnH.name(), cPgpWarnHT.name(), headerFont, signWarnBColorName)
        + QStringLiteral(
              "table.signErr {\n"
              "  background-color: %1 ! important;\n"
              "}\n\n"

              "tr.signErrH {\n"
              "  background-color: %2 ! important;\n"
              "  color: %3 ! important;\n"
              "%4"
              "}\n\n"

              "tr.signErrB { background-color: %5 ! important; }\n\n")
              .arg(cPgpErrF.name(), cPgpErrH.name(), cPgpErrHT.name(), headerFont, cPgpErrBColorName)
        + inlineMessageCss
        + QStringLiteral(
              "div.header {\n"
              "%1"
              "}\n\n"

              "%2"

              "%3"

              "div.senderpic{\n"
              "  padding: 0px ! important;\n"
              "  font-size:0.8em ! important;\n"
              "  border:1px solid %5 ! important;\n"
              "  background-color:%4 ! important;\n"
              "}\n\n"

              "div.senderstatus{\n"
              "  text-align:center ! important;\n"
              "}\n\n")

              .arg(headerFont, scrollBarCss, extraScreenCss(headerFont), pal.color(QPalette::Highlight).name(), pal.color(QPalette::Window).name())
        + quoteCSS + fullAddressList();
}

QString CSSHelperBase::commonCssDefinitions() const
{
    const QString headerFont = defaultScreenHeaderFont();

    QStringList inlineMessageCss;
    inlineMessageCss.reserve(MESSAGE_TYPE_COUNT);
    for (const auto &msgStyle : inlineMessageStyles) {
        inlineMessageCss.push_back(QLatin1StringView("div.") + QString::fromLatin1(msgStyle.cssName));
    }
    return QStringLiteral(
               "div.header {\n"
               "  margin-bottom: 10pt ! important;\n"
               "}\n\n"

               "pre.highlightattachment {\n"
               "  white-space: pre-wrap;\n"
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

               "%1"

               "%2 {\n"
               "  margin: 0px 5% 10px 5% ! important;\n"
               "  padding: 10px ! important;\n"
               "  text-align: left ! important;\n"
               "  line-height: normal;\n"
               "  min-height: %6px;\n"
               "}\n\n"

               "hr {\n"
               "  border: 0;\n"
               "  height: 0;\n"
               "  border-top: 1px solid rgba(%3, %4, %5, 0.3);\n"
               "}\n\n"

               "div.quotelevelmark {\n"
               "  position: absolute;\n"
               "  margin-left:-10px;\n"
               "}\n\n")
        .arg(extraCommonCss(headerFont))
        .arg(inlineMessageCss.join(QLatin1StringView(", ")))
        .arg(mForegroundColor.red())
        .arg(mForegroundColor.green())
        .arg(mForegroundColor.blue())
        .arg(QString::number(48));
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
