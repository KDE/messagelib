/*
  SPDX-FileCopyrightText: 2017 Sandro Kanuß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "simpleobjecttreesource.h"

#include <MimeTreeParser/BodyPartFormatterFactory>
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeSource>

using namespace MimeTreeParser;

namespace MimeTreeParser
{
class SimpleObjectTreeSourcePrivate
{
public:
    bool mDecryptMessage = false;
    Util::HtmlMode mPreferredMode = Util::Html;
};
}

SimpleObjectTreeSource::SimpleObjectTreeSource()
    : d(new SimpleObjectTreeSourcePrivate)
{
}

SimpleObjectTreeSource::~SimpleObjectTreeSource() = default;

bool SimpleObjectTreeSource::autoImportKeys() const
{
    return true;
}

const BodyPartFormatterFactory *SimpleObjectTreeSource::bodyPartFormatterFactory()
{
    return BodyPartFormatterFactory::instance();
}

bool SimpleObjectTreeSource::decryptMessage() const
{
    return d->mDecryptMessage;
}

void SimpleObjectTreeSource::setDecryptMessage(bool decryptMessage)
{
    d->mDecryptMessage = decryptMessage;
}

void SimpleObjectTreeSource::setPreferredMode(MimeTreeParser::Util::HtmlMode mode)
{
    d->mPreferredMode = mode;
}

MimeTreeParser::Util::HtmlMode SimpleObjectTreeSource::preferredMode() const
{
    return d->mPreferredMode;
}

const QTextCodec *SimpleObjectTreeSource::overrideCodec()
{
    return nullptr;
}

void SimpleObjectTreeSource::setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes)
{
    Q_UNUSED(mode)
    Q_UNUSED(availableModes)
}
