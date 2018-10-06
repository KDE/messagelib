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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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

#include "csshelper.h"
#include "settings/messageviewersettings.h"

#include "MessageCore/MessageCoreSettings"
#include <MessageCore/MessageCoreUtil>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <KColorScheme>
#include <QApplication>

#include <QColor>
#include <QFont>
#include <QPalette>

using namespace MessageViewer;

CSSHelper::CSSHelper(const QPaintDevice *pd)
    : CSSHelperBase(pd)
{
    // initialize with defaults - should match the corresponding application defaults
    mForegroundColor = QApplication::palette().color(QPalette::Text);
    mLinkColor = MessageCore::ColorUtil::self()->linkColor();
    mBackgroundColor = QApplication::palette().color(QPalette::Base);
    cHtmlWarning = QColor(0xFF, 0x40, 0x40);   // warning frame color: light red

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
    mBodyFont
        = MessageCore::MessageCoreSettings::self()->useDefaultFonts() ? defaultFont : MessageViewer
          ::
          MessageViewerSettings::self()->bodyFont();
    mPrintFont
        = MessageCore::MessageCoreSettings::self()->useDefaultFonts() ? defaultFont : MessageViewer
          ::
          MessageViewerSettings::self()->printFont();
    mFixedFont = mFixedPrintFont = defaultFixedFont;
    defaultFont.setItalic(true);
    mQuoteFont = defaultFont;

    KConfig *config = MessageViewer::MessageViewerSettings::self()->config();

    KConfigGroup reader(config, "Reader");
    KConfigGroup fonts(config, "Fonts");

    mRecycleQuoteColors = reader.readEntry("RecycleQuoteColors", false);

    mForegroundColor = KColorScheme(QPalette::Active).foreground().color();
    if (!MessageCore::MessageCoreSettings::self()->useDefaultColors()) {
        mLinkColor
            = reader.readEntry("LinkColor", mLinkColor);
        cPgpEncrH
            = reader.readEntry("PGPMessageEncr", cPgpEncrH);
        cPgpOk1H
            = reader.readEntry("PGPMessageOkKeyOk", cPgpOk1H);
        cPgpOk0H
            = reader.readEntry("PGPMessageOkKeyBad", cPgpOk0H);
        cPgpWarnH
            = reader.readEntry("PGPMessageWarn", cPgpWarnH);
        cPgpErrH
            = reader.readEntry("PGPMessageErr", cPgpErrH);
        mQuoteColor[0] = MessageCore::MessageCoreSettings::self()->quotedText1();
        mQuoteColor[1] = MessageCore::MessageCoreSettings::self()->quotedText2();
        mQuoteColor[2] = MessageCore::MessageCoreSettings::self()->quotedText3();
    } else {
        mQuoteColor[0] = MessageCore::ColorUtil::self()->quoteLevel1DefaultTextColor();
        mQuoteColor[1] = MessageCore::ColorUtil::self()->quoteLevel2DefaultTextColor();
        mQuoteColor[2] = MessageCore::ColorUtil::self()->quoteLevel3DefaultTextColor();
    }


    if (!MessageCore::MessageCoreSettings::self()->useDefaultFonts()) {
        mBodyFont = fonts.readEntry("body-font", mBodyFont);
        mPrintFont = fonts.readEntry("print-font", mPrintFont);
        mFixedFont = fonts.readEntry("fixed-font", mFixedFont);
        mFixedPrintFont = mFixedFont; // FIXME when we have a separate fixed print font
    }

    mShrinkQuotes = MessageViewer::MessageViewerSettings::self()->shrinkQuotes();

    recalculatePGPColors();
}

CSSHelper::~CSSHelper()
{
}

QString CSSHelper::htmlHead(bool fixed) const
{
    return
        QLatin1String("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n"
                      "<html><head><title></title><style>\n")
        + cssDefinitions(fixed)
        +QLatin1String("</style></head>\n"
                       "<body>\n");
}
