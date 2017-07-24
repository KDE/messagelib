/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>
  Copyright (c) 2017 Laurent Montel <montel@kde.org>

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

#include "utiltest.h"

#include <QTextDocument>
#include <qtest.h>
#include <QStandardPaths>

#include <MessageComposer/Util>
using namespace MessageComposer;

QTEST_APPLESS_MAIN(UtilTest)

UtilTest::UtilTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void UtilTest::testSelectCharset()
{
    // Pick a charset that actually works.
    {
        QString text = QStringLiteral("text 123 ăîşţâ");
        QList<QByteArray> charsets;
        charsets << "us-ascii";
        charsets << "iso-8859-1";
        charsets << "iso-8859-2"; // This one works.
        QByteArray choice = Util::selectCharset(charsets, text);
        QCOMPARE(choice, QByteArrayLiteral("iso-8859-2"));
    }

    // Pick as simple a charset as possible.
    {
        QString text = QStringLiteral("plain English text");
        QList<QByteArray> charsets;
        charsets << "us-ascii"; // This one works.
        charsets << "iso-8859-1";
        charsets << "utf-8";
        QByteArray choice = Util::selectCharset(charsets, text);
        QCOMPARE(choice, QByteArrayLiteral("us-ascii"));
    }

    // Return empty charset if none works.
    {
        QString text = QStringLiteral("text 123 ăîşţâ");
        QList<QByteArray> charsets;
        QByteArray choice = Util::selectCharset(charsets, text);
        QVERIFY(choice.isEmpty());
        charsets << "us-ascii";
        charsets << "iso-8859-1";
        choice = Util::selectCharset(charsets, text);
        QVERIFY(choice.isEmpty());
    }

}

void UtilTest::shouldTestHasMissingAttachment_data()
{
    QTest::addColumn<QStringList>("attachmentKeywords");
    QTest::addColumn<QString>("subject");
    QTest::addColumn<QString>("body");
    QTest::addColumn<bool>("hasMissingAttachment");

    QStringList lstDefaultAttachement{QStringLiteral("attachment"), QStringLiteral("att:")};
    QTest::newRow("emptybody") << lstDefaultAttachement << QStringLiteral("foo") << QString() << false;
    QTest::newRow("emptybodyandsubject") << lstDefaultAttachement << QString() << QString() << false;
}

void UtilTest::shouldTestHasMissingAttachment()
{
    QFETCH(QStringList, attachmentKeywords);
    QFETCH(QString, subject);
    QFETCH(QString, body);
    QFETCH(bool, hasMissingAttachment);

    QTextDocument doc(body);
    QCOMPARE(Util::hasMissingAttachments(attachmentKeywords, &doc, subject), hasMissingAttachment);
}


