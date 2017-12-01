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

#ifndef MIMETREEPARSER_TESTS_TESTOBJECTTREESOURCE_H
#define MIMETREEPARSER_TESTS_TESTOBJECTTREESOURCE_H

#include <MimeTreeParser/BodyPartFormatterFactory>
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeSource>

namespace MimeTreeParser {
namespace Test {
class TestObjectTreeSource : public MimeTreeParser::Interface::ObjectTreeSource
{
public:
    TestObjectTreeSource()
        : mBodyPartFormatterFactory(MimeTreeParser::BodyPartFormatterFactory::instance())
        , mPreferredMode(Util::Html)
        , mDecryptMessage(false)
    {
    }

    bool autoImportKeys() const override
    {
        return true;
    }

    const BodyPartFormatterFactory *bodyPartFormatterFactory() override
    {
        return mBodyPartFormatterFactory;
    }

    bool decryptMessage() const override
    {
        return mDecryptMessage;
    }

    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override
    {
        Q_UNUSED(mode);
        Q_UNUSED(availableModes);
    }

    MimeTreeParser::Util::HtmlMode preferredMode() const override
    {
        return mPreferredMode;
    }

    const QTextCodec *overrideCodec() override
    {
        return nullptr;
    }

    void render(const MessagePart::Ptr &msgPart, bool showOnlyOneMimePart) override
    {
        Q_UNUSED(msgPart);
        Q_UNUSED(showOnlyOneMimePart);
    }

    BodyPartFormatterFactory *mBodyPartFormatterFactory;
    MimeTreeParser::Util::HtmlMode mPreferredMode;
    bool mDecryptMessage;
};
}
}
#endif
