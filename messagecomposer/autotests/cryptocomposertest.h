/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

namespace MessageComposer
{
class Composer;
}

class CryptoComposerTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    // openpgp
    void testOpenPGPMime();
    void testOpenPGPMime_data();

    // the following will do for s-mime as well, as the same sign/enc jobs are used
    void testEncryptSameAttachments();
    void testEncryptSameAttachments_data();
    void testSignEncryptLateAttachments();
    void testSignEncryptLateAttachments_data();

    void testEditEncryptAttachments();
    void testEditEncryptAttachments_data();

    void testEditEncryptAndLateAttachments();
    void testEditEncryptAndLateAttachments_data();

    // test protected headers
    void testProtectedHeaders();
    void testProtectedHeaders_data();

    // secondary recipients
    void testBCCEncrypt();
    void testBCCEncrypt_data();

    // inline pgp
    void testOpenPGPInline_data();
    void testOpenPGPInline();

    // s-mime
    void testSMIME_data();
    void testSMIME();
    void testSMIMEOpaque_data();
    void testSMIMEOpaque();

    // contentTransferEncoding
    void testCTEquPr_data();
    void testCTEquPr();
    void testCTEbase64_data();
    void testCTEbase64();

    void testSetGnupgHome_data();
    void testSetGnupgHome();

    void testAutocryptHeaders_data();
    void testAutocryptHeaders();

    void testAutocryptGossip_data();
    void testAutocryptGossip();

    // TODO test the code for autodetecting the charset of a text attachment.
private:
    void fillComposerData(MessageComposer::Composer *composer, const QString &data);
    void fillComposerCryptoData(MessageComposer::Composer *composer);

    // convenience, shared code
    void runSMIMETest(bool sign, bool enc, bool opaque);
};
