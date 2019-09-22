/*
  Copyright (C) 2017 Sandro Kanuß <sknauss@kde.org>

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

#include "simpleobjecttreesource.h"

#include <MimeTreeParser/BodyPartFormatterFactory>
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeSource>

using namespace MimeTreeParser;

namespace MimeTreeParser {
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
    Q_UNUSED(mode);
    Q_UNUSED(availableModes);
}
