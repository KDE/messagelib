/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

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

#include "mailwebenginescripttest.h"
#include "../mailwebenginescript.h"

#include <QTest>

MailWebEngineScriptTest::MailWebEngineScriptTest(QObject *parent)
    : QObject(parent)
{
}

MailWebEngineScriptTest::~MailWebEngineScriptTest()
{
}

void MailWebEngineScriptTest::shouldInjectAttachment()
{
    const QString expectedResult = QStringLiteral("qt.jQuery('#bla').append('foo')");
    QString script = MessageViewer::MailWebEngineScript::injectAttachments(QStringLiteral(
                                                                               "foo"),
                                                                           QStringLiteral("bla"));
    // skip the checks at the beginning of the script
    script = script.mid(script.indexOf(QLatin1Char('\n')) + 1);
    script = script.mid(script.indexOf(QLatin1Char('\n')) + 1);
    QCOMPARE(script, expectedResult);
}

void MailWebEngineScriptTest::shouldReplaceInnerHtml()
{
    const QString expectedResult = QStringLiteral(
        "qt.jQuery('#iconFullGGAddressList').append('<a>hello word</a>')");
    QString script = MessageViewer::MailWebEngineScript::replaceInnerHtml(QStringLiteral(
                                                                              "GG"),
                                                                          QStringLiteral(
                                                                              "<a>hello word</a>"));
    // skip the checks at the beginning of the script
    script = script.mid(script.indexOf(QLatin1Char('\n')) + 1);
    QCOMPARE(script, expectedResult);
}

QTEST_MAIN(MailWebEngineScriptTest)
