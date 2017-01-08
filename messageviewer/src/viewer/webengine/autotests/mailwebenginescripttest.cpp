/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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
    const QString result = QStringLiteral("qt.jQuery('#bla').append('foo')");
    QCOMPARE(MessageViewer::MailWebEngineScript::injectAttachments(QStringLiteral("foo"), QStringLiteral("bla")), result);
}

void MailWebEngineScriptTest::shouldReplaceInnerHtml()
{
    const QString result = QStringLiteral("qt.jQuery('#iconFullGGAddressList').append('<a>hello word</a>')");
    QCOMPARE(MessageViewer::MailWebEngineScript::replaceInnerHtml(QStringLiteral("GG"), QStringLiteral("<a>hello word</a>")), result);
}

QTEST_MAIN(MailWebEngineScriptTest)
