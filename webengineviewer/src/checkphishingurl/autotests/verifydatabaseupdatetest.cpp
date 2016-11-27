/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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

VerifyDataBaseUpdateTest::~VerifyDataBaseUpdateTest()
{

}

void VerifyDataBaseUpdateTest::shouldVerifyCheckSums_data()
{
    QTest::addColumn<QList<WebEngineViewer::Addition> >("additionList");
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

    QByteArray calculateCheckSums = QByteArrayLiteral("\xBC\xB3\xEDk\xE3x\xD1(\xA9\xEDz7]x\x18\xBDn]"
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
    Q_FOREACH (const WebEngineViewer::Addition &add, additionList) {
        const QByteArray uncompressed = add.hashString;
        for (int i = 0; i < uncompressed.size();) {
            const QByteArray m = uncompressed.mid(i, add.prefixSize);
            i += add.prefixSize;

            WebEngineViewer::Addition tmp;
            tmp.hashString = m;
            tmp.prefixSize = add.prefixSize;
            itemToStore << tmp;

            if (m.size() != add.prefixSize) {
                qDebug() << "m " << m << " m.size" << m.size();
            }
        }
    }
    QCOMPARE(itemToStore.count(), numberOfItems);

    QByteArray newSsha256;
    std::sort(itemToStore.begin(), itemToStore.end(), WebEngineViewer::Addition::lessThan);
    Q_FOREACH (const WebEngineViewer::Addition &add, itemToStore) {
        QByteArray ba = add.hashString;
        newSsha256 += ba;
    }
    QCOMPARE(newSsha256, QByteArrayLiteral("----11112222254321abcdabcdebbbbbcdefefgh"));

    const QByteArray newSsha256Value = QCryptographicHash::hash(newSsha256, QCryptographicHash::Sha256);
    QCOMPARE(newSsha256Value, calculateCheckSums);
    QCOMPARE(newSsha256Value.toBase64(), calculateCheckSums.toBase64());
}

QTEST_MAIN(VerifyDataBaseUpdateTest)
