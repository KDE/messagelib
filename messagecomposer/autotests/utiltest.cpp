/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "utiltest.h"

#include <QStandardPaths>
#include <QTest>
#include <QTextDocument>

#include <MessageComposer/Util>
using namespace MessageComposer;

QTEST_MAIN(UtilTest)

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
        QVector<QByteArray> charsets;
        charsets << "us-ascii";
        charsets << "iso-8859-1";
        charsets << "iso-8859-2"; // This one works.
        QByteArray choice = Util::selectCharset(charsets, text);
        QCOMPARE(choice, QByteArrayLiteral("iso-8859-2"));
    }

    // Pick as simple a charset as possible.
    {
        QString text = QStringLiteral("plain English text");
        QVector<QByteArray> charsets;
        charsets << "us-ascii"; // This one works.
        charsets << "iso-8859-1";
        charsets << "utf-8";
        QByteArray choice = Util::selectCharset(charsets, text);
        QCOMPARE(choice, QByteArrayLiteral("us-ascii"));
    }

    // Return empty charset if none works.
    {
        QString text = QStringLiteral("text 123 ăîşţâ");
        QVector<QByteArray> charsets;
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

    QStringList lstDefaultAttachement{QStringLiteral("attachment"), QStringLiteral("att2")};
    QTest::newRow("emptybody") << lstDefaultAttachement << QStringLiteral("foo") << QString() << false;
    QTest::newRow("emptybodyandsubject") << lstDefaultAttachement << QString() << QString() << false;
    QTest::newRow("subjectwithattachmentkeyword") << lstDefaultAttachement << QStringLiteral("attachment foo") << QString() << true;
    QTest::newRow("subjectwithattachmentkeywordonly") << lstDefaultAttachement << QStringLiteral("attachment") << QString() << true;
    QTest::newRow("subjectwithattachmentkeywordbutreply") << lstDefaultAttachement << QStringLiteral("Re: attachment") << QString() << false;
    QTest::newRow("subjectwithattachmentkeywordbutreplywithoutattachement")
        << lstDefaultAttachement << QStringLiteral("Re: attachment") << QStringLiteral("foo bla\n bli") << false;
    QTest::newRow("subjectwithattachmentkeywordbutreplywithattachement")
        << lstDefaultAttachement << QStringLiteral("Re: attachment") << QStringLiteral("foo bla\n attachment:") << true;
    QTest::newRow("subjectwithattachmentkeywordbutreplywithattachement2")
        << lstDefaultAttachement << QStringLiteral("Re: attachment") << QStringLiteral("foo bla att2\n bli:") << true;
    QTest::newRow("excludequotedstr") << lstDefaultAttachement << QStringLiteral("Re: attachment") << QStringLiteral("> foo bla att2\n bli:") << false;
    QTest::newRow("excludequotedstr2") << lstDefaultAttachement << QStringLiteral("Re: attachment") << QStringLiteral("> foo bla att2\n att2:") << true;
    QTest::newRow("excludequotedstr3") << lstDefaultAttachement << QStringLiteral("Re: attachment") << QStringLiteral("| foo bla att2\n att2:") << true;
    QTest::newRow("subjectwithattachmentkeywordbutforward") << lstDefaultAttachement << QStringLiteral("Fwd: attachment") << QString() << false;
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
