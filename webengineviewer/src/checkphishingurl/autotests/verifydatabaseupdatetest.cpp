/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "verifydatabaseupdatetest.h"
#include "../updatedatabaseinfo.h"
#include <QCryptographicHash>
#include <QTest>

Q_DECLARE_METATYPE(QList<WebEngineViewer::Addition>)

VerifyDataBaseUpdateTest::VerifyDataBaseUpdateTest(QObject *parent)
    : QObject(parent)
{
}

VerifyDataBaseUpdateTest::~VerifyDataBaseUpdateTest() = default;

void VerifyDataBaseUpdateTest::shouldVerifyCheckSums_data()
{
    QTest::addColumn<QList<WebEngineViewer::Addition>>("additionList");
    QTest::addColumn<int>("numberOfItems");
    QTest::addColumn<QByteArray>("calculateCheckSums");

    WebEngineViewer::Addition a;
    a.hashString = QByteArray("----1111bbbb");
    a.prefixSize = 4;

    WebEngineViewer::Addition b;
    b.hashString = QByteArray("abcdefgh");
    b.prefixSize = 4;

    WebEngineViewer::Addition c;
    c.hashString = QByteArray("54321abcde");
    c.prefixSize = 5;

    WebEngineViewer::Addition d;
    d.hashString = QByteArray("22222bcdef");
    d.prefixSize = 5;

    QList<WebEngineViewer::Addition> lst;
    lst << a << b << c << d;

    QByteArray calculateCheckSums = QByteArrayLiteral(
        "\xBC\xB3\xEDk\xE3x\xD1(\xA9\xEDz7]x\x18\xBDn]"
        "\xA5\xA8R\xF7\xAB\xCF\xC1\xA3\xA3\xC5Z,\xA6o");

    QTest::newRow("checksum1") << lst << 9 << calculateCheckSums;

    lst.clear();

    WebEngineViewer::Addition a1;
    a1.hashString = QByteArray("----1111bbbbabcdefgh");
    a1.prefixSize = 4;
    WebEngineViewer::Addition c1;
    c1.hashString = QByteArray("54321abcde22222bcdef");
    c1.prefixSize = 5;

    lst << a1 << c1;
    QTest::newRow("checksum2") << lst << 9 << calculateCheckSums;

    lst.clear();

    WebEngineViewer::Addition c2;
    c2.hashString = QByteArray("54321abcde22222bcdef");
    c2.prefixSize = 5;

    WebEngineViewer::Addition a2;
    a2.hashString = QByteArray("----1111bbbbabcdefgh");
    a2.prefixSize = 4;

    lst << c2 << a2;
    QTest::newRow("checksum3") << lst << 9 << calculateCheckSums;
}

void VerifyDataBaseUpdateTest::shouldVerifyCheckSums()
{
    QFETCH(QList<WebEngineViewer::Addition>, additionList);
    QFETCH(int, numberOfItems);
    QFETCH(QByteArray, calculateCheckSums);

    // Proof of checksum validity using python:
    // >>> import hashlib
    // >>> m = hashlib.sha256()
    // >>> m.update("----11112222254321abcdabcdebbbbbcdefefgh")
    // >>> m.digest()
    // "\xbc\xb3\xedk\xe3x\xd1(\xa9\xedz7]"
    // "x\x18\xbdn]\xa5\xa8R\xf7\xab\xcf\xc1\xa3\xa3\xc5Z,\xa6o"

    QList<WebEngineViewer::Addition> itemToStore;
    for (const WebEngineViewer::Addition &add : additionList) {
        const QByteArray uncompressed = add.hashString;
        for (int i = 0; i < uncompressed.size();) {
            const QByteArray m = uncompressed.mid(i, add.prefixSize);
            i += add.prefixSize;

            WebEngineViewer::Addition tmp;
            tmp.hashString = m;
            tmp.prefixSize = add.prefixSize;
            itemToStore << tmp;

            if (m.size() != add.prefixSize) {
                qDebug() << "hashstring: " << m << " hash size: " << m.size();
            }
        }
    }
    QCOMPARE(itemToStore.count(), numberOfItems);

    QByteArray newSsha256;
    std::sort(itemToStore.begin(), itemToStore.end(), WebEngineViewer::Addition::lessThan);
    for (const WebEngineViewer::Addition &add : std::as_const(itemToStore)) {
        QByteArray ba = add.hashString;
        newSsha256 += ba;
    }
    QCOMPARE(newSsha256, QByteArrayLiteral("----11112222254321abcdabcdebbbbbcdefefgh"));

    const QByteArray newSsha256Value = QCryptographicHash::hash(newSsha256, QCryptographicHash::Sha256);
    QCOMPARE(newSsha256Value, calculateCheckSums);
    QCOMPARE(newSsha256Value.toBase64(), calculateCheckSums.toBase64());
}

QTEST_GUILESS_MAIN(VerifyDataBaseUpdateTest)

#include "moc_verifydatabaseupdatetest.cpp"
