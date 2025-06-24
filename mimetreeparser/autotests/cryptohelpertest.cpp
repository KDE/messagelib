/* SPDX-FileCopyrightText: 2015 Sandro Knauß <knauss@kolabsys.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "cryptohelpertest.h"
using namespace Qt::Literals::StringLiterals;

#include "cryptohelper.h"

#include <QTest>

using namespace MimeTreeParser;

void CryptoHelperTest::testPMFDEmpty()
{
    QCOMPARE(prepareMessageForDecryption("").count(), 0);
}

void CryptoHelperTest::testPMFDWithNoPGPBlock()
{
    const QByteArray text = "testblabla";
    const QList<Block> blocks = prepareMessageForDecryption(text);
    QCOMPARE(blocks.count(), 1);
    QCOMPARE(blocks[0].text(), text);
    QCOMPARE(blocks[0].type(), NoPgpBlock);
}

void CryptoHelperTest::testPGPBlockType()
{
    const QString blockText = u"text"_s;
    const QString preString = u"before\n"_s;
    for (int i = 1; i <= PrivateKeyBlock; ++i) {
        QString name;
        switch (i) {
        case PgpMessageBlock:
            name = u"MESSAGE"_s;
            break;
        case MultiPgpMessageBlock:
            name = u"MESSAGE PART"_s;
            break;
        case SignatureBlock:
            name = u"SIGNATURE"_s;
            break;
        case ClearsignedBlock:
            name = u"SIGNED MESSAGE"_s;
            break;
        case PublicKeyBlock:
            name = u"PUBLIC KEY BLOCK"_s;
            break;
        case PrivateKeyBlock:
            name = u"PRIVATE KEY BLOCK"_s;
            break;
        }
        QString text = QLatin1StringView("-----BEGIN PGP ") + name + u'\n' + blockText;
        QList<Block> blocks = prepareMessageForDecryption(preString.toLatin1() + text.toLatin1());
        QCOMPARE(blocks.count(), 1);
        QCOMPARE(blocks[0].type(), UnknownBlock);

        text += QLatin1StringView("\n-----END PGP ") + name + u'\n';
        blocks = prepareMessageForDecryption(preString.toLatin1() + text.toLatin1());
        QCOMPARE(blocks.count(), 2);
        QCOMPARE(blocks[1].text(), text.toLatin1());
        QCOMPARE(blocks[1].type(), static_cast<PGPBlockType>(i));
    }
}

void CryptoHelperTest::testDeterminePGPBlockType()
{
    const QString blockText = u"text"_s;
    for (int i = 1; i <= PrivateKeyBlock; ++i) {
        QString name;
        switch (i) {
        case PgpMessageBlock:
            name = u"MESSAGE"_s;
            break;
        case MultiPgpMessageBlock:
            name = u"MESSAGE PART"_s;
            break;
        case SignatureBlock:
            name = u"SIGNATURE"_s;
            break;
        case ClearsignedBlock:
            name = u"SIGNED MESSAGE"_s;
            break;
        case PublicKeyBlock:
            name = u"PUBLIC KEY BLOCK"_s;
            break;
        case PrivateKeyBlock:
            name = u"PRIVATE KEY BLOCK"_s;
            break;
        }
        const QString text = QLatin1StringView("-----BEGIN PGP ") + name + u'\n' + blockText + u'\n';
        const Block block = Block(text.toLatin1());
        QCOMPARE(block.text(), text.toLatin1());
        QCOMPARE(block.type(), static_cast<PGPBlockType>(i));
    }
}

void CryptoHelperTest::testEmbededPGPBlock()
{
    const QByteArray text = QByteArray("before\n-----BEGIN PGP MESSAGE-----\ncrypted - you see :)\n-----END PGP MESSAGE-----\nafter");
    const QList<Block> blocks = prepareMessageForDecryption(text);
    QCOMPARE(blocks.count(), 3);
    QCOMPARE(blocks[0].text(), QByteArray("before\n"));
    QCOMPARE(blocks[1].text(), QByteArray("-----BEGIN PGP MESSAGE-----\ncrypted - you see :)\n-----END PGP MESSAGE-----\n"));
    QCOMPARE(blocks[2].text(), QByteArray("after"));
}

void CryptoHelperTest::testClearSignedMessage()
{
    const QByteArray text = QByteArray(
        "before\n-----BEGIN PGP SIGNED MESSAGE-----\nsigned content\n-----BEGIN PGP SIGNATURE-----\nfancy signature\n-----END PGP SIGNATURE-----\nafter");
    const QList<Block> blocks = prepareMessageForDecryption(text);
    QCOMPARE(blocks.count(), 3);
    QCOMPARE(blocks[0].text(), QByteArray("before\n"));
    QCOMPARE(blocks[1].text(),
             QByteArray("-----BEGIN PGP SIGNED MESSAGE-----\nsigned content\n-----BEGIN PGP SIGNATURE-----\nfancy signature\n-----END PGP SIGNATURE-----\n"));
    QCOMPARE(blocks[2].text(), QByteArray("after"));
}

void CryptoHelperTest::testMultipleBlockMessage()
{
    const QByteArray text = QByteArray(
        "before\n-----BEGIN PGP SIGNED MESSAGE-----\nsigned content\n-----BEGIN PGP SIGNATURE-----\nfancy signature\n-----END PGP "
        "SIGNATURE-----\nafter\n-----BEGIN PGP MESSAGE-----\ncrypted - you see :)\n-----END PGP MESSAGE-----\n");
    const QList<Block> blocks = prepareMessageForDecryption(text);
    QCOMPARE(blocks.count(), 4);
    QCOMPARE(blocks[0].text(), QByteArray("before\n"));
    QCOMPARE(blocks[1].text(),
             QByteArray("-----BEGIN PGP SIGNED MESSAGE-----\nsigned content\n-----BEGIN PGP SIGNATURE-----\nfancy signature\n-----END PGP SIGNATURE-----\n"));
    QCOMPARE(blocks[2].text(), QByteArray("after\n"));
    QCOMPARE(blocks[3].text(), QByteArray("-----BEGIN PGP MESSAGE-----\ncrypted - you see :)\n-----END PGP MESSAGE-----\n"));
}

QTEST_APPLESS_MAIN(CryptoHelperTest)

#include "moc_cryptohelpertest.cpp"
