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
#include <viewer/objecttreeemptysource.h>
#include <viewer/attachmentstrategy.h>

namespace MessageViewer
{

namespace Test
{

/**
* setup a environment variables for tests:
* * set LC_ALL to C
* * set KDEHOME
*/
void setupEnv();

// We can't use EmptySource, since we need to control some emelnets of the source for tests to also test
// loadExternal and htmlMail.
class TestObjectTreeSource : public MessageViewer::EmptySource
{
public:
    TestObjectTreeSource(MimeTreeParser::HtmlWriter *writer,
                         MimeTreeParser::CSSHelperBase *cssHelper)
        : mWriter(writer)
        , mCSSHelper(cssHelper)
        , mAttachmentStrategy(QStringLiteral("smart"))
        , mHtmlLoadExternal(false)
        , mHtmlMail(true)
    {
    }

    MimeTreeParser::HtmlWriter *htmlWriter() Q_DECL_OVERRIDE {
        return mWriter;
    }
    MimeTreeParser::CSSHelperBase *cssHelper() Q_DECL_OVERRIDE {
        return mCSSHelper;
    }

    bool htmlLoadExternal() const Q_DECL_OVERRIDE
    {
        return mHtmlLoadExternal;
    }

    void setHtmlLoadExternal(bool loadExternal)
    {
        mHtmlLoadExternal = loadExternal;
    }

    bool htmlMail() const Q_DECL_OVERRIDE
    {
        return mHtmlMail;
    }

    void setHtmlMail(bool htmlMail)
    {
        mHtmlMail = htmlMail;
    }

    void setAttachmentStrategy(QString strategy)
    {
        mAttachmentStrategy = strategy;
    }

    const MimeTreeParser::AttachmentStrategy *attachmentStrategy() Q_DECL_OVERRIDE {
        return  MimeTreeParser::AttachmentStrategy::create(mAttachmentStrategy);
    }

    bool autoImportKeys() const Q_DECL_OVERRIDE
    {
        return true;
    }

    bool showEmoticons() const Q_DECL_OVERRIDE
    {
        return false;
    }

    bool showExpandQuotesMark() const Q_DECL_OVERRIDE
    {
        return false;
    }

private:
    MimeTreeParser::HtmlWriter *mWriter;
    MimeTreeParser::CSSHelperBase *mCSSHelper;
    QString mAttachmentStrategy;
    bool mHtmlLoadExternal;
    bool mHtmlMail;
};

}

}

#endif
