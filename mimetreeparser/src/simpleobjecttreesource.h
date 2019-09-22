/*
  Copyright (C) 2017 Sandro Kanuß <sknauss@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef MIMETREEPARSER_SIMPLEOBJECTTREESOURCE_H
#define MIMETREEPARSER_SIMPLEOBJECTTREESOURCE_H

#include "mimetreeparser/objecttreesource.h"

#include <memory>

namespace MimeTreeParser {
class SimpleObjectTreeSourcePrivate;

/**
 * A very simple ObjectTreeSource.
 * mostly used in tests and outside the read redering
 */
class MIMETREEPARSER_EXPORT SimpleObjectTreeSource : public Interface::ObjectTreeSource
{
public:
    SimpleObjectTreeSource();
    ~SimpleObjectTreeSource() override;

    bool autoImportKeys() const override;

    const BodyPartFormatterFactory *bodyPartFormatterFactory() override;
    bool decryptMessage() const override;
    void setDecryptMessage(bool decryptMessage);

    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;

    void setPreferredMode(MimeTreeParser::Util::HtmlMode mode);
    MimeTreeParser::Util::HtmlMode preferredMode() const override;

    const QTextCodec *overrideCodec() override;

private:
    std::unique_ptr<SimpleObjectTreeSourcePrivate> d;
};
}
#endif
