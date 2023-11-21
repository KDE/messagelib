/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "objecttreeemptysource.h"
#include "viewer/attachmentstrategy.h"
#include "viewer/csshelperbase.h"
#include "viewer/viewer_p.h"

#include <MimeTreeParser/BodyPartFormatter>
#include <MimeTreeParser/BodyPartFormatterFactory>

#include "messagepartthemes/default/defaultrenderer.h"

#include "messageviewer_debug.h"

using namespace MessageViewer;

namespace MessageViewer
{
class EmptySourcePrivate
{
public:
    EmptySourcePrivate() = default;

    bool mAllowDecryption = false;
};
}

EmptySource::EmptySource()
    : MimeTreeParser::Interface::ObjectTreeSource()
    , d(new MessageViewer::EmptySourcePrivate)
{
}

EmptySource::~EmptySource() = default;

bool EmptySource::decryptMessage() const
{
    return d->mAllowDecryption;
}

bool EmptySource::htmlLoadExternal() const
{
    return false;
}

void EmptySource::setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes)
{
    Q_UNUSED(mode)
    Q_UNUSED(availableModes)
}

MimeTreeParser::Util::HtmlMode EmptySource::preferredMode() const
{
    return MimeTreeParser::Util::Html;
}

void EmptySource::setAllowDecryption(bool allowDecryption)
{
    d->mAllowDecryption = allowDecryption;
}

QByteArray EmptySource::overrideCodecName() const
{
    return {};
}

QString EmptySource::createMessageHeader(KMime::Message *message)
{
    Q_UNUSED(message)
    return {}; // do nothing
}

const AttachmentStrategy *EmptySource::attachmentStrategy() const
{
    return AttachmentStrategy::smart();
}

HtmlWriter *EmptySource::htmlWriter() const
{
    return nullptr;
}

CSSHelperBase *EmptySource::cssHelper() const
{
    return nullptr;
}

bool EmptySource::autoImportKeys() const
{
    return true;
}

const MimeTreeParser::BodyPartFormatterFactory *EmptySource::bodyPartFormatterFactory()
{
    return MimeTreeParser::BodyPartFormatterFactory::instance();
}

void EmptySource::render(const MimeTreeParser::MessagePartPtr &msgPart, bool showOnlyOneMimePart)
{
    if (!htmlWriter()) {
        qCWarning(MESSAGEVIEWER_LOG) << "no htmlWriter - skipping rendering.";
        return;
    }

    auto renderer = DefaultRenderer(cssHelper());
    renderer.setAttachmentStrategy(attachmentStrategy());
    renderer.setCreateMessageHeader(std::bind(&EmptySource::createMessageHeader, this, std::placeholders::_1));
    renderer.setHtmlLoadExternal(htmlLoadExternal());
    renderer.setIsPrinting(isPrinting());
    renderer.setLevelQuote(levelQuote());
    renderer.setShowEmoticons(showEmoticons());
    renderer.setShowExpandQuotesMark(showExpandQuotesMark());
    renderer.setShowOnlyOneMimePart(showOnlyOneMimePart);
    renderer.setShowSignatureDetails(showSignatureDetails());
    renderer.setShowEncryptionDetails(showEncryptionDetails());

    renderer.render(msgPart, htmlWriter());
}

bool EmptySource::isPrinting() const
{
    return false;
}

bool EmptySource::showEmoticons() const
{
    return false;
}

bool EmptySource::showExpandQuotesMark() const
{
    return false;
}

bool EmptySource::showSignatureDetails() const
{
    return false;
}

bool EmptySource::showEncryptionDetails() const
{
    return false;
}

int EmptySource::levelQuote() const
{
    return 1;
}
