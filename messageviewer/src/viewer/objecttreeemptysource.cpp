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

#include "objecttreeemptysource.h"
#include "viewer/viewer_p.h"
#include "viewer/csshelperbase.h"

#include <MimeTreeParser/AttachmentStrategy>
#include <MimeTreeParser/BodyPartFormatter>

#include "messagepartthemes/default/defaultrenderer.h"

#include "bodypartformatterfactorysingleton.h"

using namespace MessageViewer;

namespace MessageViewer {
class EmptySourcePrivate
{
public:
    EmptySourcePrivate()
        : mAllowDecryption(false)
    {
    }

    bool mAllowDecryption;
};
}

EmptySource::EmptySource()
    : MimeTreeParser::Interface::ObjectTreeSource()
    , d(new MessageViewer::EmptySourcePrivate)
{
}

EmptySource::~EmptySource()
{
    delete d;
}

bool EmptySource::decryptMessage() const
{
    return d->mAllowDecryption;
}

bool EmptySource::htmlLoadExternal() const
{
    return false;
}

bool EmptySource::showSignatureDetails() const
{
    return false;
}

void EmptySource::setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes)
{
    Q_UNUSED(mode);
    Q_UNUSED(availableModes);
}

MimeTreeParser::Util::HtmlMode EmptySource::preferredMode() const
{
    return MimeTreeParser::Util::Html;
}

void EmptySource::setAllowDecryption(bool allowDecryption)
{
    d->mAllowDecryption = allowDecryption;
}

int EmptySource::levelQuote() const
{
    return 1;
}

const QTextCodec *EmptySource::overrideCodec()
{
    return nullptr;
}

QString EmptySource::createMessageHeader(KMime::Message *message)
{
    Q_UNUSED(message);
    return QString(); //do nothing
}

const MimeTreeParser::AttachmentStrategy *EmptySource::attachmentStrategy()
{
    return MimeTreeParser::AttachmentStrategy::smart();
}

MimeTreeParser::HtmlWriter *EmptySource::htmlWriter()
{
    return nullptr;
}

CSSHelperBase *EmptySource::cssHelper()
{
    return nullptr;
}

bool EmptySource::autoImportKeys() const
{
    return true;
}

bool EmptySource::showEmoticons() const
{
    return false;
}

bool EmptySource::showExpandQuotesMark() const
{
    return false;
}

const MimeTreeParser::BodyPartFormatterBaseFactory *EmptySource::bodyPartFormatterFactory()
{
    return bodyPartFormatterBaseFactoryInstance();
}

bool EmptySource::isPrinting() const
{
    return false;
}

MimeTreeParser::Interface::MessagePartRendererPtr EmptySource::messagePartTheme(
    MimeTreeParser::Interface::MessagePart::Ptr msgPart)
{
    return MimeTreeParser::Interface::MessagePartRenderer::Ptr(new DefaultRenderer(msgPart,
                                                                                   cssHelper()));
}
