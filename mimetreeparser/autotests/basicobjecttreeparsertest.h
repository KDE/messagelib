/*
  Copyright (c) 2010 Thomas McGuire <thomas.mcguire@kdab.com>
  Copyright (c) 2019 Sandro Knauß <sknauss@kde.org>

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

#ifndef MIMETREEPARSER_TESTS_OBJECTTREEPARSERTEST_H
#define MIMETREEPARSER_TESTS_OBJECTTREEPARSERTEST_H

#include <QObject>

class ObjectTreeParserTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testMailWithoutEncryption();
    void testSMIMESignedEncrypted();
    void testOpenPGPSignedEncrypted();
    void testOpenPGPEncryptedAndSigned();
    void testForwardedOpenPGPSignedEncrypted();
    void testSignedForwardedOpenPGPSignedEncrypted();
    void testOpenPGPEncrypted();
    void testOpenPGPEncryptedNotDecrypted();
    void testAsync_data();
    void testAsync();
    void testHtmlContent_data();
    void testHtmlContent();
    void testMemoryHole();
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

#endif // MIMETREEPARSER_TESTS_OBJECTTREEPARSERTEST_H
