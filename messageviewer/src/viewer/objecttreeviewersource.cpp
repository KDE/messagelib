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
#include "bodypartformatterfactorysingleton.h"

#include "viewer/viewer_p.h"
#include "widgets/htmlstatusbar.h"
#include "settings/messageviewersettings.h"

namespace MessageViewer
{
MailViewerSource::MailViewerSource(ViewerPrivate *viewer) :
    MimeTreeParser::ObjectTreeSourceIf(), mViewer(viewer)
{
}

MailViewerSource::~MailViewerSource()
{
}

bool MailViewerSource::htmlMail() const
{
    return mViewer->htmlMail();
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

void MailViewerSource::setHtmlMode(MimeTreeParser::Util::HtmlMode mode)
{
    mViewer->mColorBar->setMode(mode);
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
    return mViewer->writeMsgHeader(message);
}

QObject *MailViewerSource::sourceObject()
{
    return mViewer;
}

const MimeTreeParser::AttachmentStrategy *MailViewerSource::attachmentStrategy()
{
    return mViewer->attachmentStrategy();
}

MimeTreeParser::HtmlWriter *MailViewerSource::htmlWriter()
{
    return mViewer->htmlWriter();
}

MimeTreeParser::CSSHelperBase *MailViewerSource::cssHelper()
{
    return mViewer->cssHelper();
}

bool MailViewerSource::autoImportKeys() const
{
    return MessageViewer::MessageViewerSettings::self()->autoImportKeys();
}

bool MailViewerSource::showEmoticons() const
{
    return MessageViewer::MessageViewerSettings::self()->showEmoticons();
}

bool MailViewerSource::showExpandQuotesMark() const
{
    return MessageViewer::MessageViewerSettings::self()->showExpandQuotesMark();
}

const MimeTreeParser::BodyPartFormatterBaseFactory *MailViewerSource::bodyPartFormatterFactory()
{
    return bodyPartFormatterBaseFactoryInstance();
}
}
