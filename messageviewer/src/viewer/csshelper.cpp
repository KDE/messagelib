/*
    csshelper.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "csshelper.h"
#include "settings/messageviewersettings.h"

#include <MessageCore/ColorUtil>
#include <MessageCore/MessageCoreSettings>

#include <KColorScheme>
#include <KConfig>
#include <KConfigGroup>
#include <QApplication>

#include <QFont>
#include <QPalette>

using namespace MessageViewer;

static const struct {
    CSSHelperBase::InlineMessageType type;
    KColorScheme::ForegroundRole role;
} inlineMessageColors[] = {{CSSHelperBase::Positive, KColorScheme::PositiveText},
                           {CSSHelperBase::Information, KColorScheme::ActiveText},
                           {CSSHelperBase::Warning, KColorScheme::NeutralText},
                           {CSSHelperBase::Error, KColorScheme::NegativeText}};

CSSHelper::CSSHelper(const QPaintDevice *pd)
    : CSSHelperBase(pd)
{
    // initialize with defaults - should match the corresponding application defaults
    updateColor();
    mLinkColor = MessageCore::ColorUtil::self()->linkColor();
    for (const auto &msgColor : inlineMessageColors) {
        cInlineMessage[msgColor.type] = KColorScheme(QPalette::Active).foreground(msgColor.role).color();
    }

    cPgpEncrH = MessageCore::ColorUtil::self()->pgpEncryptedMessageColor();
    cPgpEncrHT = MessageCore::ColorUtil::self()->pgpEncryptedTextColor();
    cPgpOk1H = MessageCore::ColorUtil::self()->pgpSignedTrustedMessageColor();
    cPgpOk1HT = MessageCore::ColorUtil::self()->pgpSignedTrustedTextColor();
    cPgpOk0H = MessageCore::ColorUtil::self()->pgpSignedUntrustedMessageColor();
    cPgpOk0HT = MessageCore::ColorUtil::self()->pgpSignedUntrustedTextColor();
    cPgpWarnH = MessageCore::ColorUtil::self()->pgpSignedUntrustedMessageColor();
    cPgpWarnHT = MessageCore::ColorUtil::self()->pgpSignedUntrustedTextColor();
    cPgpErrH = MessageCore::ColorUtil::self()->pgpSignedBadMessageColor();
    cPgpErrHT = MessageCore::ColorUtil::self()->pgpSignedBadTextColor();

    mRecycleQuoteColors = false;

    QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    QFont defaultFixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    mBodyFont = MessageCore::MessageCoreSettings::self()->useDefaultFonts() ? defaultFont : MessageViewer ::MessageViewerSettings::self()->bodyFont();
    mPrintFont = MessageCore::MessageCoreSettings::self()->useDefaultFonts() ? defaultFont : MessageViewer ::MessageViewerSettings::self()->printFont();
    mFixedFont = mFixedPrintFont = defaultFixedFont;
    defaultFont.setItalic(true);
    mQuoteFont = defaultFont;

    KConfig *config = MessageViewer::MessageViewerSettings::self()->config();

    KConfigGroup reader(config, QStringLiteral("Reader"));
    KConfigGroup fonts(config, QStringLiteral("Fonts"));

    mRecycleQuoteColors = reader.readEntry("RecycleQuoteColors", false);

    mForegroundColor = KColorScheme(QPalette::Active).foreground().color();
    if (MessageCore::MessageCoreSettings::self()->useDefaultColors()) {
        mQuoteColor[0] = MessageCore::ColorUtil::self()->quoteLevel1DefaultTextColor();
        mQuoteColor[1] = MessageCore::ColorUtil::self()->quoteLevel2DefaultTextColor();
        mQuoteColor[2] = MessageCore::ColorUtil::self()->quoteLevel3DefaultTextColor();
    } else {
        mLinkColor = reader.readEntry("LinkColor", mLinkColor);
        cPgpEncrH = reader.readEntry("PGPMessageEncr", cPgpEncrH);
        cPgpOk1H = reader.readEntry("PGPMessageOkKeyOk", cPgpOk1H);
        cPgpOk0H = reader.readEntry("PGPMessageOkKeyBad", cPgpOk0H);
        cPgpWarnH = reader.readEntry("PGPMessageWarn", cPgpWarnH);
        cPgpErrH = reader.readEntry("PGPMessageErr", cPgpErrH);
        mQuoteColor[0] = MessageCore::MessageCoreSettings::self()->quotedText1();
        mQuoteColor[1] = MessageCore::MessageCoreSettings::self()->quotedText2();
        mQuoteColor[2] = MessageCore::MessageCoreSettings::self()->quotedText3();
    }

    if (!MessageCore::MessageCoreSettings::self()->useDefaultFonts()) {
        mBodyFont = fonts.readEntry("body-font", mBodyFont);
        mPrintFont = fonts.readEntry("print-font", mPrintFont);
        mFixedFont = fonts.readEntry("fixed-font", mFixedFont);
        mFixedPrintFont = mFixedFont;
    }

    mShrinkQuotes = MessageViewer::MessageViewerSettings::self()->shrinkQuotes();

    mUseBrowserColor = MessageCore::MessageCoreSettings::self()->useRealHtmlMailColor();
    recalculatePGPColors();
}

CSSHelper::~CSSHelper() = default;

void CSSHelper::updateColor()
{
    // KColorScheme(QPalette::Active).background().color();

    mForegroundColor = KColorScheme(QPalette::Active).foreground().color(); // QApplication::palette().color(QPalette::Text);
    mBackgroundColor = KColorScheme(QPalette::Active).background().color(); // QApplication::palette().color(QPalette::Base);
}

QString CSSHelper::htmlHead(const HtmlHeadSettings &htmlHeadSettings) const
{
    return QLatin1StringView(
               "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n"
               "<html><head><title></title><style>\n")
        + cssDefinitions(htmlHeadSettings)
        + QLatin1StringView(
               "</style></head>\n"
               "<body>\n");
}

QString CSSHelper::endBodyHtml() const
{
    return QStringLiteral("</body></html>");
}
