/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "utiltest.h"
using namespace Qt::Literals::StringLiterals;

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

void UtilTest::shouldTestHasMissingAttachment_data()
{
    QTest::addColumn<QStringList>("attachmentKeywords");
    QTest::addColumn<QString>("subject");
    QTest::addColumn<QString>("body");
    QTest::addColumn<bool>("hasMissingAttachment");

    QStringList lstDefaultAttachement{u"attachment"_s, QStringLiteral("att2")};
    QTest::newRow("emptybody") << lstDefaultAttachement << u"foo"_s << QString() << false;
    QTest::newRow("emptybodyandsubject") << lstDefaultAttachement << QString() << QString() << false;
    QTest::newRow("subjectwithattachmentkeyword") << lstDefaultAttachement << u"attachment foo"_s << QString() << true;
    QTest::newRow("subjectwithattachmentkeywordonly") << lstDefaultAttachement << u"attachment"_s << QString() << true;
    QTest::newRow("subjectwithattachmentkeywordbutreply") << lstDefaultAttachement << u"Re: attachment"_s << QString() << false;
    QTest::newRow("subjectwithattachmentkeywordbutreplywithoutattachement")
        << lstDefaultAttachement << u"Re: attachment"_s << QStringLiteral("foo bla\n bli") << false;
    QTest::newRow("subjectwithattachmentkeywordbutreplywithattachement")
        << lstDefaultAttachement << u"Re: attachment"_s << QStringLiteral("foo bla\n attachment:") << true;
    QTest::newRow("subjectwithattachmentkeywordbutreplywithattachement2")
        << lstDefaultAttachement << u"Re: attachment"_s << QStringLiteral("foo bla att2\n bli:") << true;
    QTest::newRow("excludequotedstr") << lstDefaultAttachement << u"Re: attachment"_s << QStringLiteral("> foo bla att2\n bli:") << false;
    QTest::newRow("excludequotedstr2") << lstDefaultAttachement << u"Re: attachment"_s << QStringLiteral("> foo bla att2\n att2:") << true;
    QTest::newRow("excludequotedstr3") << lstDefaultAttachement << u"Re: attachment"_s << QStringLiteral("| foo bla att2\n att2:") << true;
    QTest::newRow("subjectwithattachmentkeywordbutforward") << lstDefaultAttachement << u"Fwd: attachment"_s << QString() << false;
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

#include "moc_utiltest.cpp"
