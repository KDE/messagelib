/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "cryptofunctions.h"

#include "setupenv.h"

#include <MessageComposer/Util>

#include <KMime/Content>
#include <KMime/Message>

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <QTest>
#include <cstdlib>
#include <gpgme++/keylistresult.h>

using namespace MessageComposer;

void ComposerTestUtil::verify(bool sign,
                              bool encrypt,
                              KMime::Content *content,
                              const QByteArray &origContent,
                              Kleo::CryptoMessageFormat f,
                              KMime::Headers::contentEncoding encoding)
{
    if (sign && encrypt) {
        verifySignatureAndEncryption(content, origContent, f, false);
    } else if (sign) {
        verifySignature(content, origContent, f, encoding);
    } else {
        verifyEncryption(content, origContent, f);
    }
}

void ComposerTestUtil::verifySignature(KMime::Content *content,
                                       const QByteArray &signedContent,
                                       Kleo::CryptoMessageFormat f,
                                       KMime::Headers::contentEncoding encoding)
{
    // store it in a KMime::Message, that's what OTP needs
    auto resultMessage = new KMime::Message;
    resultMessage->setContent(content->encodedContent());
    resultMessage->parse();

    // parse the result and make sure it is valid in various ways
    MimeTreeParser::SimpleObjectTreeSource testSource;
    auto nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    // ensure the signed part exists and is parseable
    if (f & Kleo::OpenPGPMIMEFormat) {
        // process the result..
        otp.parseObjectTree(resultMessage);

        KMime::Content *signedPart = Util::findTypeInMessage(resultMessage, "application", "pgp-signature");
        Q_ASSERT(signedPart);
        QCOMPARE(signedPart->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);
        Q_UNUSED(signedPart)

        // Q_ASSERT( nh->signatureState( resultMessage ) == MimeTreeParser::KMMsgFullySigned );

        QVERIFY(!resultMessage->contents().isEmpty());
        QCOMPARE(resultMessage->contents().at(0)->contentTransferEncoding()->encoding(), encoding);
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
            QCOMPARE(signedPart->contentType(false)->name(), QStringLiteral("smime.p7s"));
            QCOMPARE(signedPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
            QCOMPARE(signedPart->contentDisposition()->filename(), QStringLiteral("smime.p7s"));
            Q_UNUSED(signedPart)

            QVERIFY(!resultMessage->contents().isEmpty());
            QCOMPARE(resultMessage->contents().at(0)->contentTransferEncoding()->encoding(), encoding);

            QCOMPARE(resultMessage->contentType()->mimeType(), QByteArray("multipart/signed"));
            QCOMPARE(resultMessage->contentType(false)->parameter("protocol"), QStringLiteral("application/pkcs7-signature"));
            QCOMPARE(resultMessage->contentType(false)->parameter("micalg"), QStringLiteral("sha1"));
        } else if (f & Kleo::SMIMEOpaqueFormat) {
            KMime::Content *signedPart = Util::findTypeInMessage(resultMessage, "application", "pkcs7-mime");
            Q_ASSERT(signedPart);
            QCOMPARE(signedPart->contentTransferEncoding()->encoding(), KMime::Headers::CEbase64);
            QCOMPARE(signedPart->contentType()->mimeType(), QByteArray("application/pkcs7-mime"));
            QCOMPARE(signedPart->contentType(false)->name(), QStringLiteral("smime.p7m"));
            QCOMPARE(signedPart->contentType(false)->parameter("smime-type"), QStringLiteral("signed-data"));
            QCOMPARE(signedPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
            QCOMPARE(signedPart->contentDisposition(false)->filename(), QStringLiteral("smime.p7m"));
            Q_UNUSED(signedPart)
        }
        // process the result..
        otp.parseObjectTree(resultMessage);

        // Q_ASSERT( nh->signatureState( resultMessage ) == MimeTreeParser::KMMsgFullySigned );
    }

    // make sure the good sig is of what we think it is
    QCOMPARE(otp.plainTextContent().trimmed(), QString::fromUtf8(signedContent).trimmed());
    Q_UNUSED(signedContent)
}

void ComposerTestUtil::verifyEncryption(KMime::Content *content, const QByteArray &encrContent, Kleo::CryptoMessageFormat f, bool withAttachment)
{
    // store it in a KMime::Message, that's what OTP needs
    KMime::Message::Ptr resultMessage(new KMime::Message);
    resultMessage->setContent(content->encodedContent());
    resultMessage->parse();

    // parse the result and make sure it is valid in various ways
    MimeTreeParser::SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    auto nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    if (f & Kleo::OpenPGPMIMEFormat) {
        // ensure the enc part exists and is parseable
        KMime::Content *encPart = Util::findTypeInMessage(resultMessage.data(), "application", "pgp-encrypted");
        Q_ASSERT(encPart);
        Q_UNUSED(encPart)

        // process the result..
        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);
    } else if (f & Kleo::InlineOpenPGPFormat) {
        if (withAttachment) {
            // Only first MimePart is the encrypted Text
            KMime::Content *cContent = resultMessage->contents().at(0);
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
        QCOMPARE(encPart->contentType(false)->name(), QStringLiteral("smime.p7m"));
        QCOMPARE(encPart->contentType(false)->parameter("smime-type"), QStringLiteral("enveloped-data"));
        QCOMPARE(encPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
        QCOMPARE(encPart->contentDisposition(false)->filename(), QStringLiteral("smime.p7m"));
        Q_UNUSED(encPart)

        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);
    }
    QCOMPARE(otp.plainTextContent().trimmed(), QString::fromUtf8(encrContent).trimmed());
    Q_UNUSED(encrContent)
}

void ComposerTestUtil::verifySignatureAndEncryption(KMime::Content *content,
                                                    const QByteArray &origContent,
                                                    Kleo::CryptoMessageFormat f,
                                                    bool withAttachment,
                                                    bool combined)
{
    Q_UNUSED(withAttachment)
    // store it in a KMime::Message, that's what OTP needs
    KMime::Message::Ptr resultMessage(new KMime::Message);
    resultMessage->setContent(content->encodedContent());
    resultMessage->parse();

    // parse the result and make sure it is valid in various ways
    MimeTreeParser::SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    auto nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    if (f & Kleo::OpenPGPMIMEFormat) {
        // ensure the enc part exists and is parseable
        KMime::Content *encPart = Util::findTypeInMessage(resultMessage.data(), "application", "pgp-encrypted");
        Q_ASSERT(encPart);
        Q_UNUSED(encPart)
        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

        QList<KMime::Content *> extra = nh->extraContents(resultMessage.data());
        QCOMPARE(extra.size(), 1);
        if (combined) {
            QCOMPARE(nh->signatureState(resultMessage.data()), MimeTreeParser::KMMsgFullySigned);
        } else {
            QCOMPARE(extra.size(), 1);
            QCOMPARE(nh->signatureState(extra[0]), MimeTreeParser::KMMsgFullySigned);
        }
    } else if (f & Kleo::InlineOpenPGPFormat) {
        otp.parseObjectTree(resultMessage.data());

        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);
        QCOMPARE(nh->signatureState(resultMessage.data()), MimeTreeParser::KMMsgFullySigned);
    } else if (f & Kleo::AnySMIME) {
        KMime::Content *encPart = Util::findTypeInMessage(resultMessage.data(), "application", "pkcs7-mime");
        Q_ASSERT(encPart);
        QCOMPARE(encPart->contentType()->mimeType(), QByteArray("application/pkcs7-mime"));
        QCOMPARE(encPart->contentType(false)->name(), QStringLiteral("smime.p7m"));
        QCOMPARE(encPart->contentType(false)->parameter("smime-type"), QStringLiteral("enveloped-data"));
        QCOMPARE(encPart->contentDisposition()->disposition(), KMime::Headers::CDattachment);
        QCOMPARE(encPart->contentDisposition(false)->filename(), QStringLiteral("smime.p7m"));
        Q_UNUSED(encPart)

        otp.parseObjectTree(resultMessage.data());
        QCOMPARE(nh->encryptionState(resultMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

        QList<KMime::Content *> extra = nh->extraContents(resultMessage.data());
        QCOMPARE(extra.size(), 1);
        QCOMPARE(nh->signatureState(extra[0]), MimeTreeParser::KMMsgFullySigned);
    }
    QCOMPARE(otp.plainTextContent().trimmed(), QString::fromUtf8(origContent).trimmed());
    Q_UNUSED(origContent)
}
