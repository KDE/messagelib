/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <MimeTreeParser/BodyPartFormatterFactory>
#include <MimeTreeParser/ObjectTreeSource>
#include <gpgme++/key.h>

#include <MessageViewer/AttachmentStrategy>
#include <MessageViewer/ObjectTreeEmptySource>

namespace MessageViewer
{
namespace Test
{
/**
 * setup a environment variables for tests:
 * * set LC_ALL to en_US.UTF-8
 */
void setupEnv();

// We can't use EmptySource, since we need to control some elements of the source for tests to also test
// loadExternal and htmlMail.
class ObjectTreeSource : public MessageViewer::EmptySource
{
public:
    ObjectTreeSource(HtmlWriter *writer, MessageViewer::CSSHelperBase *cssHelper)
        : mWriter(writer)
        , mCSSHelper(cssHelper)
        , mAttachmentStrategy(QStringLiteral("smart"))
        , mHtmlLoadExternal(false)
        , mDecryptMessage(false)
        , mShowSignatureDetails(false)
        , mShowEncryptionDetails(false)
        , mShowExpandQuotesMark(false)
        , mPreferredMode(MimeTreeParser::Util::Html)
        , mQuoteLevel(1)
    {
    }

    HtmlWriter *htmlWriter() const override
    {
        return mWriter;
    }

    CSSHelperBase *cssHelper() const override
    {
        return mCSSHelper;
    }

    bool htmlLoadExternal() const override
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

    const AttachmentStrategy *attachmentStrategy() const override
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

    void setShowExpandQuotesMark(bool b)
    {
        mShowExpandQuotesMark = b;
    }

    bool showExpandQuotesMark() const override
    {
        return mShowExpandQuotesMark;
    }

    const MimeTreeParser::BodyPartFormatterFactory *bodyPartFormatterFactory() override
    {
        return &mBodyPartFormatterFactory;
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

    void setShowEncryptionDetails(bool showEncryptionDetails)
    {
        mShowEncryptionDetails = showEncryptionDetails;
    }

    bool showEncryptionDetails() const override
    {
        return mShowEncryptionDetails;
    }

    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override
    {
        Q_UNUSED(mode)
        Q_UNUSED(availableModes)
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
        return mQuoteLevel;
    }

    void setLevelQuote(int level)
    {
        mQuoteLevel = level;
    }

    QByteArray overrideCodecName() const override
    {
        return {};
    }

    QString createMessageHeader(KMime::Message *message) override
    {
        Q_UNUSED(message)
        return {}; // do nothing
    }

private:
    HtmlWriter *mWriter;
    MessageViewer::CSSHelperBase *mCSSHelper;
    QString mAttachmentStrategy;
    MimeTreeParser::BodyPartFormatterFactory mBodyPartFormatterFactory;
    bool mHtmlLoadExternal;
    bool mDecryptMessage;
    bool mShowSignatureDetails;
    bool mShowEncryptionDetails;
    bool mShowExpandQuotesMark;
    MimeTreeParser::Util::HtmlMode mPreferredMode;
    int mQuoteLevel;
};
}
}
