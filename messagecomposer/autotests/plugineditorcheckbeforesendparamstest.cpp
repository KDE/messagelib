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

#include "plugineditorcheckbeforesendparamstest.h"
#include <MessageComposer/PluginEditorCheckBeforeSendParams>
#include <QTest>

PluginEditorCheckBeforeSendParamsTest::PluginEditorCheckBeforeSendParamsTest(QObject *parent)
    : QObject(parent)
{

}

PluginEditorCheckBeforeSendParamsTest::~PluginEditorCheckBeforeSendParamsTest()
{

}

void PluginEditorCheckBeforeSendParamsTest::shouldHaveDefaultValues()
{
    MessageComposer::PluginEditorCheckBeforeSendParams params;
    QVERIFY(params.subject().isEmpty());
    QCOMPARE(params.identity(), (uint)-1);
    QVERIFY(!params.isHtmlMail());
}

void PluginEditorCheckBeforeSendParamsTest::shouldBeEqual()
{
    MessageComposer::PluginEditorCheckBeforeSendParams params1;
    const QString subject(QStringLiteral("foo"));
    bool isHmlMail = true;
    params1.setSubject(subject);
    params1.setHtmlMail(isHmlMail);
    MessageComposer::PluginEditorCheckBeforeSendParams params2 = params1;
    QVERIFY(params2 == params1);
    QCOMPARE(params2.isHtmlMail(), isHmlMail);
    QCOMPARE(params2.subject(), subject);
}

QTEST_MAIN(PluginEditorCheckBeforeSendParamsTest)
