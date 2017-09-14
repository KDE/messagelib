/*
  Copyright (C) 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Leo Franchi <lfranchi@kde.org>

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

#include "cryptofunctions.h"

#include "testhtmlwriter.h"
#include "testcsshelper.h"

#include "MessageComposer/Util"

#include <Libkleo/Enum>
#include <KMime/Message>
#include <KMime/Content>

#include <MessageViewer/CSSHelper>
#include <MessageViewer/ObjectTreeEmptySource>
#include <MimeTreeParser/ObjectTreeParser>

#include <MessageCore/NodeHelper>

#include <QtTest>
#include <stdlib.h>
#include <gpgme++/keylistresult.h>
#include <setupenv.h>

using namespace MessageComposer;

void ComposerTestUtil::verify(bool sign, bool encrypt, KMime::Content *content, const QByteArray &origContent, Kleo::CryptoMessageFormat f, KMime::Headers::contentEncoding encoding)
{
    if (sign && encrypt) {
        verifySignatureAndEncryption(content, origContent, f, false);
    } else if (sign) {
        verifySignature(content, origContent, f, encoding);
    } else {
        verifyEncryption(content, origContent, f);
    }
}

void ComposerTestUtil::verifySignature(KMime::Content *content, const QByteArray &signedContent, Kleo::CryptoMessageFormat f, KMime::Headers::contentEncoding encoding)
{
    // store it in a KMime::Message, that's what OTP needs
    KMime::Message *resultMessage =  new KMime::Message;
    resultMessage->setContent(content->encodedContent());
    resultMessage->parse();

    // parse the result and make sure it is valid in various ways
    TestHtmlWriter testWriter;
    TestCSSHelper testCSSHelper;
    MessageComposer::Test::TestObjectTreeSource testSource(&testWriter, &testCSSHelper);
    MimeTreeParser::NodeHelper *nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);
    MimeTreeParser::ProcessResult pResult(nh);

    // ensure the signed part exists and is parseable
    if (f & Kleo::OpenPGPMIMEFormat) {
        // process the result..
        otp.parseObjectTree(resultMessage);

        KMime::Content *signedPart = Util::findTypeInMessage(resultMessage, "application", "pgp-signature");
        Q_ASSERT(signedPart);
        QCOMPARE(signedPart->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);
        Q_UNUSED(signedPart);

        //Q_ASSERT( nh->signatureState( resultMessage ) == MimeTreeParser::KMMsgFullySigned );

        QCOMPARE(MessageCore::NodeHelper::firstChild(resultMessage)->contentTransferEncoding()->encoding(), encoding);
    } else if (f & Kleo::InlineOpenPGPFormat) {
        // process the result..
        otp.parseObjectTree(resultMessage);
        QCOMPARE(nh->signatureState(resultMessage), MimeTreeParser::KMMsgFullySigned);

        QCOMPARE(resultMessage->contentTransferEncoding()->encoding(), encoding);
    } else if (f & Kleo::AnySMIME) {
        if (f & Kleo::SMIMEFormat) {
            KMime::Content *signedPart = Util::findTypeInMessage(resultMessage, "application", "pkcs7-signature");
            Q_ASSERT(signedPart);
            QCOMPARE(signedPart->contentTransferEncoding()->encoding(), KMime::Headers::CEbase64);
            QCOMPARE(signedPart->contentType()->mimeType(), QByteArray("application/pkcs7-signature"));
            QCOMPARE(signedPart->contentType()->name(), QString::fromLatin1("smime.p7s"));
            QCOMPARE(signedPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
            QCOMPARE(signedPart->contentDisposition()->filename(), QString::fromLatin1("smime.p7s"));
            Q_UNUSED(signedPart);

            QCOMPARE(MessageCore::NodeHelper::firstChild(resultMessage)->contentTransferEncoding()->encoding(), encoding);

            QCOMPARE(resultMessage->contentType()->mimeType(), QByteArray("multipart/signed"));
            QCOMPARE(resultMessage->contentType()->parameter(QString::fromLatin1("protocol")), QString::fromLatin1("application/pkcs7-signature"));
            QCOMPARE(resultMessage->contentType()->parameter(QString::fromLatin1("micalg")), QString::fromLatin1("sha1"));

        } else if (f & Kleo::SMIMEOpaqueFormat) {
            KMime::Content *signedPart = Util::findTypeInMessage(resultMessage, "application", "pkcs7-mime");
            Q_ASSERT(signedPart);
            QCOMPARE(signedPart->contentTransferEncoding()->encoding(), KMime::Headers::CEbase64);
            QCOMPARE(signedPart->contentType()->mimeType(), QByteArray("application/pkcs7-mime"));
            QCOMPARE(signedPart->contentType()->name(), QString::fromLatin1("smime.p7m"));
            QCOMPARE(signedPart->contentType()->parameter(QString::fromLatin1("smime-type")), QString::fromLatin1("signed-data"));
            QCOMPARE(signedPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
            QCOMPARE(signedPart->contentDisposition()->filename(), QString::fromLatin1("smime.p7m"));
            Q_UNUSED(signedPart);
        }
        // process the result..
        otp.parseObjectTree(resultMessage);

        //Q_ASSERT( nh->signatureState( resultMessage ) == MimeTreeParser::KMMsgFullySigned );
    }

    // make sure the good sig is of what we think it is
    QCOMPARE(otp.plainTextContent().trimmed(), QString::fromUtf8(signedContent).trimmed());
    Q_UNUSED(signedContent);

}

void ComposerTestUtil::verifyEncryption(KMime::Content *content, const QByteArray &encrContent, Kleo::CryptoMessageFormat f, bool withAttachment)
{
    // store it in a KMime::Message, that's what OTP needs
    KMime::Message::Ptr resultMessage = KMime::Message::Ptr(new KMime::Message);
    resultMessage->setContent(content->encodedContent());
    resultMessage->parse();

    // parse the result and make sure it is valid in various ways
    TestHtmlWriter testWriter;
    TestCSSHelper testCSSHelper;
    MessageComposer::Test::TestObjectTreeSource testSource(&testWriter, &testCSSHelper);
    testSource.setAllowDecryption(true);
    MimeTreeParser::NodeHelper *nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    if (f & Kleo::OpenPGPMIMEFormat) {
        // ensure the enc part exists and is parseable
        KMime::Content *encPart = Util::findTypeInMessage(resultMessage.data(), "application", "pgp-encrypted");
        Q_ASSERT(encPart);
        Q_UNUSED(encPart);

        // process the result..
        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

    } else if (f & Kleo::InlineOpenPGPFormat) {
        if (withAttachment) {
            //Only first MimePart is the encrypted Text
            KMime::Content *cContent = MessageCore::NodeHelper::firstChild(resultMessage.data());
            resultMessage->setContent(cContent->encodedContent());
            resultMessage->parse();
        }

        otp.parseObjectTree(resultMessage.data());

        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);
    } else if (f & Kleo::AnySMIME) {
        // ensure the enc part exists and is parseable
        KMime::Content *encPart = Util::findTypeInMessage(resultMessage.data(), "application", "pkcs7-mime");
        Q_ASSERT(encPart);

        QCOMPARE(encPart->contentType()->mimeType(), QByteArray("application/pkcs7-mime"));
        QCOMPARE(encPart->contentType()->name(), QString::fromLatin1("smime.p7m"));
        QCOMPARE(encPart->contentType()->parameter(QString::fromLatin1("smime-type")), QString::fromLatin1("enveloped-data"));
        QCOMPARE(encPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
        QCOMPARE(encPart->contentDisposition()->filename(), QString::fromLatin1("smime.p7m"));
        Q_UNUSED(encPart);

        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

    }
    QCOMPARE(otp.plainTextContent().trimmed(), QString::fromUtf8(encrContent).trimmed());
    Q_UNUSED(encrContent);
}

void ComposerTestUtil::verifySignatureAndEncryption(KMime::Content *content, const QByteArray &origContent, Kleo::CryptoMessageFormat f, bool withAttachment)
{
    Q_UNUSED(withAttachment);
    // store it in a KMime::Message, that's what OTP needs
    KMime::Message::Ptr resultMessage =  KMime::Message::Ptr(new KMime::Message);
    resultMessage->setContent(content->encodedContent());
    resultMessage->parse();

    // parse the result and make sure it is valid in various ways
    TestHtmlWriter testWriter;
    TestCSSHelper testCSSHelper;
    MessageComposer::Test::TestObjectTreeSource testSource(&testWriter, &testCSSHelper);
    testSource.setAllowDecryption(true);
    MimeTreeParser::NodeHelper *nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    if (f & Kleo::OpenPGPMIMEFormat) {
        // ensure the enc part exists and is parseable
        KMime::Content *encPart = Util::findTypeInMessage(resultMessage.data(), "application", "pgp-encrypted");
        Q_ASSERT(encPart);
        Q_UNUSED(encPart);
        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

        QList< KMime::Content * > extra = nh->extraContents(resultMessage.data());
        QCOMPARE(extra.size(), 1);
        QCOMPARE(nh->signatureState(extra[ 0 ]), MimeTreeParser::KMMsgFullySigned);
    } else if (f & Kleo::InlineOpenPGPFormat) {
        otp.parseObjectTree(resultMessage.data());

        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);
        QCOMPARE(nh->signatureState(resultMessage.data()), MimeTreeParser::KMMsgFullySigned);
    } else if (f & Kleo::AnySMIME) {
        KMime::Content *encPart = Util::findTypeInMessage(resultMessage.data(), "application", "pkcs7-mime");
        Q_ASSERT(encPart);
        QCOMPARE(encPart->contentType()->mimeType(), QByteArray("application/pkcs7-mime"));
        QCOMPARE(encPart->contentType()->name(), QString::fromLatin1("smime.p7m"));
        QCOMPARE(encPart->contentType()->parameter(QString::fromLatin1("smime-type")), QString::fromLatin1("enveloped-data"));
        QCOMPARE(encPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
        QCOMPARE(encPart->contentDisposition()->filename(), QString::fromLatin1("smime.p7m"));
        Q_UNUSED(encPart);

        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

        QList< KMime::Content * > extra = nh->extraContents(resultMessage.data());
        QCOMPARE(extra.size(), 1);
        QCOMPARE(nh->signatureState(extra[ 0 ]), MimeTreeParser::KMMsgFullySigned);
    }
    QCOMPARE(otp.plainTextContent().trimmed(), QString::fromUtf8(origContent).trimmed());
    Q_UNUSED(origContent);
}

