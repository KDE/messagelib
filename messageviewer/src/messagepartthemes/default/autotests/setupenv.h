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

#ifndef __MESSAGEVIEWER_TESTS_SETUPENV_H__
#define __MESSAGEVIEWER_TESTS_SETUPENV_H__

#include <gpgme++/key.h>

#include <MimeTreeParser/AttachmentStrategy>
#include <MimeTreeParser/BodyPartFormatterBaseFactory>
#include <MimeTreeParser/ObjectTreeSource>

#include <MessageViewer/ObjectTreeEmptySource>

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
class ObjectTreeSource : public MessageViewer::EmptySource
{
public:
    ObjectTreeSource(MimeTreeParser::HtmlWriter *writer,
                     MessageViewer::CSSHelperBase *cssHelper)
        : mWriter(writer)
        , mCSSHelper(cssHelper)
        , mAttachmentStrategy(QStringLiteral("smart"))
        , mHtmlLoadExternal(false)
        , mDecryptMessage(false)
        , mShowSignatureDetails(false)
        , mShowExpandQuotesMark(false)
        , mPreferredMode(MimeTreeParser::Util::Html)
        , mQuoteLevel(1)
    {
    }

    MimeTreeParser::HtmlWriter *htmlWriter() Q_DECL_OVERRIDE {
        return mWriter;
    }

    CSSHelperBase *cssHelper() Q_DECL_OVERRIDE {
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

    void setAttachmentStrategy(const QString &strategy)
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

    void setShowExpandQuotesMark(bool b)
    {
        mShowExpandQuotesMark = b;
    }

    bool showExpandQuotesMark() const Q_DECL_OVERRIDE
    {
        return mShowExpandQuotesMark;
    }

    const MimeTreeParser::BodyPartFormatterBaseFactory *bodyPartFormatterFactory() Q_DECL_OVERRIDE {
        return &mBodyPartFormatterBaseFactory;
    }

    bool decryptMessage() const Q_DECL_OVERRIDE
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

    bool showSignatureDetails() const Q_DECL_OVERRIDE
    {
        return mShowSignatureDetails;
    }

    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) Q_DECL_OVERRIDE {
        Q_UNUSED(mode);
        Q_UNUSED(availableModes);
    }

    MimeTreeParser::Util::HtmlMode preferredMode() const Q_DECL_OVERRIDE
    {
        return mPreferredMode;
    }

    void setPreferredMode(MimeTreeParser::Util::HtmlMode mode)
    {
        mPreferredMode = mode;
    }

    int levelQuote() const Q_DECL_OVERRIDE
    {
        return mQuoteLevel;
    }

    void setLevelQuote(int level)
    {
        mQuoteLevel = level;
    }

    const QTextCodec *overrideCodec() Q_DECL_OVERRIDE {
        return nullptr;
    }

    QString createMessageHeader(KMime::Message *message) Q_DECL_OVERRIDE {
        Q_UNUSED(message);
        return QString(); //do nothing
    }

    QObject *sourceObject() Q_DECL_OVERRIDE {
        return nullptr;
    }

private:
    MimeTreeParser::HtmlWriter *mWriter;
    MessageViewer::CSSHelperBase *mCSSHelper;
    QString mAttachmentStrategy;
    MimeTreeParser::BodyPartFormatterBaseFactory mBodyPartFormatterBaseFactory;
    bool mHtmlLoadExternal;
    bool mDecryptMessage;
    bool mShowSignatureDetails;
    bool mShowExpandQuotesMark;
    MimeTreeParser::Util::HtmlMode mPreferredMode;
    int mQuoteLevel;
};

}

}

#endif //__MESSAGEVIEWER_TESTS_SETUPENV_H__

