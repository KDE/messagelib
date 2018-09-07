/*
  Copyright (C) 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Andras Mantia <andras@kdab.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "objecttreeviewersource.h"
#include "csshelper.h"
#include <MimeTreeParser/BodyPartFormatterFactory>

#include <MimeTreeParser/BodyPartFormatter>
#include "messagepartthemes/default/defaultrenderer.h"

#include "viewer/viewer_p.h"
#include "widgets/htmlstatusbar.h"
#include "settings/messageviewersettings.h"

using namespace MessageViewer;

MailViewerSource::MailViewerSource(ViewerPrivate *viewer)
    : MessageViewer::EmptySource()
    , mViewer(viewer)
{
}

MailViewerSource::~MailViewerSource()
{
}

bool MailViewerSource::decryptMessage() const
{
    return mViewer->decryptMessage();
}

bool MailViewerSource::htmlLoadExternal() const
{
    return mViewer->htmlLoadExternal();
}

bool MailViewerSource::showSignatureDetails() const
{
    return mViewer->mShowSignatureDetails;
}

bool MailViewerSource::showEncryptionDetails() const
{
    return mViewer->mShowEncryptionDetails;
}

void MailViewerSource::setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes)
{
//    qDebug() << " void MailViewerSource::setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes)";
//    for (int i = 0; i < availableModes.count(); ++i) {
//        qDebug() << "MailViewerSource::setHtmlMode Mode " << MimeTreeParser::Util::htmlModeToString(availableModes.at(i));
//    }
//    qDebug() << "MailViewerSource::setHtmlMode preferred " << MimeTreeParser::Util::htmlModeToString(mode);

    mViewer->mColorBar->setAvailableModes(availableModes);
    mViewer->mColorBar->setMode(mode);
}

MimeTreeParser::Util::HtmlMode MailViewerSource::preferredMode() const
{
    switch (mViewer->displayFormatMessageOverwrite()) {
    case MessageViewer::Viewer::UseGlobalSetting:
    case MessageViewer::Viewer::Unknown:
        return mViewer->htmlMailGlobalSetting() ? MimeTreeParser::Util::Html : MimeTreeParser::Util
               ::Normal;
    case MessageViewer::Viewer::Html:
        return MimeTreeParser::Util::MultipartHtml;
    case MessageViewer::Viewer::Text:
        return MimeTreeParser::Util::MultipartPlain;
    case MessageViewer::Viewer::ICal:
        return MimeTreeParser::Util::MultipartIcal;
    }
    Q_ASSERT(true);
    return MimeTreeParser::Util::Html;
}

int MailViewerSource::levelQuote() const
{
    return mViewer->mLevelQuote;
}

const QTextCodec *MailViewerSource::overrideCodec()
{
    return mViewer->overrideCodec();
}

QString MailViewerSource::createMessageHeader(KMime::Message *message)
{
    return mViewer->writeMessageHeader(message);
}

const AttachmentStrategy *MailViewerSource::attachmentStrategy() const
{
    return mViewer->attachmentStrategy();
}

CSSHelperBase *MailViewerSource::cssHelper() const
{
    return mViewer->cssHelper();
}

HtmlWriter *MailViewerSource::htmlWriter() const
{
    return mViewer->htmlWriter();
}

bool MailViewerSource::autoImportKeys() const
{
    return MessageViewer::MessageViewerSettings::self()->autoImportKeys();
}

bool MailViewerSource::showEmoticons() const
{
    return mViewer->showEmoticons();
}

bool MailViewerSource::showExpandQuotesMark() const
{
    return MessageViewer::MessageViewerSettings::self()->showExpandQuotesMark();
}

bool MailViewerSource::isPrinting() const
{
    return mViewer->mPrinting;
}
