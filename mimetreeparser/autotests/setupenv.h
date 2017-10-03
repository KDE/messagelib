/*
  Copyright (C) 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>

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

#ifndef MESSAGECORE_TESTS_UTIL_H
#define MESSAGECORE_TESTS_UTIL_H

#include <gpgme++/key.h>
#include <MimeTreeParser/AttachmentStrategy>
#include <MimeTreeParser/BodyPartFormatter>
#include <MimeTreeParser/BodyPartFormatterBaseFactory>
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeSource>

namespace MimeTreeParser {
namespace Test {
/**
* setup a environment variables for tests:
* * set LC_ALL to C
* * set KDEHOME
*/
void setupEnv();

// We can't use EmptySource, since we need to control some emelnets of the source for tests to also test
// loadExternal and htmlMail.
class TestObjectTreeSource : public MimeTreeParser::Interface::ObjectTreeSource
{
public:
    TestObjectTreeSource(MimeTreeParser::HtmlWriter *writer)
        : mWriter(writer)
        , mAttachmentStrategy(QStringLiteral("smart"))
        , mPreferredMode(Util::Html)
        , mHtmlLoadExternal(false)
        , mDecryptMessage(false)
    {
    }

    MimeTreeParser::HtmlWriter *htmlWriter() override
    {
        return mWriter;
    }

    bool htmlLoadExternal() const override
    {
        return mHtmlLoadExternal;
    }

    void setHtmlLoadExternal(bool loadExternal)
    {
        mHtmlLoadExternal = loadExternal;
    }

    void setAttachmentStrategy(QString strategy)
    {
        mAttachmentStrategy = strategy;
    }

    const AttachmentStrategy *attachmentStrategy() override
    {
        return AttachmentStrategy::create(mAttachmentStrategy);
    }

    bool autoImportKeys() const override
    {
        return true;
    }

    bool showEmoticons() const override
    {
        return false;
    }

    bool showExpandQuotesMark() const override
    {
        return false;
    }

    const BodyPartFormatterBaseFactory *bodyPartFormatterFactory() override
    {
        return &mBodyPartFormatterBaseFactory;
    }

    bool decryptMessage() const override
    {
        return mDecryptMessage;
    }

    void setAllowDecryption(bool allowDecryption)
    {
        mDecryptMessage = allowDecryption;
    }

    void setShowSignatureDetails(bool showSignatureDetails)
    {
        mShowSignatureDetails = showSignatureDetails;
    }

    bool showSignatureDetails() const override
    {
        return mShowSignatureDetails;
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

    void setPreferredMode(MimeTreeParser::Util::HtmlMode mode)
    {
        mPreferredMode = mode;
    }

    int levelQuote() const override
    {
        return 1;
    }

    const QTextCodec *overrideCodec() override
    {
        return nullptr;
    }

    QString createMessageHeader(KMime::Message *message) override
    {
        Q_UNUSED(message);
        return QString(); //do nothing
    }

    void render(const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter) override
    {
        Q_UNUSED(msgPart);
        Q_UNUSED(htmlWriter);
    }

    bool isPrinting() const override
    {
        return false;
    }

private:
    MimeTreeParser::HtmlWriter *mWriter = nullptr;
    QString mAttachmentStrategy;
    BodyPartFormatterBaseFactory mBodyPartFormatterBaseFactory;
    MimeTreeParser::Util::HtmlMode mPreferredMode;
    bool mHtmlLoadExternal = false;
    bool mDecryptMessage = false;
    bool mShowSignatureDetails = false;
};
}
}

#endif
