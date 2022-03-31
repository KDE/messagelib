/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>
  SPDX-FileCopyrightText: 2019 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ObjectTreeParserTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testMailWithoutEncryption();
    void testBinaryAttachmentNotPGP();
    void testSMIMESignedEncrypted();
    void testOpenPGPSignedEncrypted();
    void testOpenPGPEncryptedAndSigned();
    void testForwardedOpenPGPSignedEncrypted();
    void testSignedForwardedOpenPGPSignedEncrypted();
    void testOpenPGPEncrypted();
    void testOpenPGPEncryptedNotDecrypted();
    void testOpenPGPEncryptedOverrideEncoding();
    void testAsync_data();
    void testAsync();
    void testHtmlContent_data();
    void testHtmlContent();
    void testRenderedTree();
    void testRenderedTree_data();
    void testParsePlainMessage();
    void testParseEncapsulatedMessage();
    void testMissingContentTypeHeader();
    void testInlinePGPDecryption();
    void testInlinePGPSigned();
    void testHTML();
    void testHTMLasText();
    void testHTMLOnly();
};
