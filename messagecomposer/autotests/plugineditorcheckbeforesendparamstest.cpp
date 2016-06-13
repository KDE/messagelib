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
    QVERIFY(params.plainText().isEmpty());
    QCOMPARE(params.identity(), (uint) - 1);
    QCOMPARE(params.transportId(), -1);
    QVERIFY(!params.isHtmlMail());
    QVERIFY(params.addresses().isEmpty());
    QVERIFY(params.defaultDomain().isEmpty());
    QVERIFY(!params.hasAttachment());
}

void PluginEditorCheckBeforeSendParamsTest::shouldBeEqual()
{
    MessageComposer::PluginEditorCheckBeforeSendParams params1;
    const QString subject(QStringLiteral("foo"));
    const QString plainText(QStringLiteral("bla"));
    const QStringList lst{ QStringLiteral("foo"), QStringLiteral("foo"), QStringLiteral("foo")};
    const QString defaultDomain(QStringLiteral("bli"));
    bool isHmlMail = true;
    bool hasAttachment = true;
    int transportId = 5;
    params1.setSubject(subject);
    params1.setHtmlMail(isHmlMail);
    params1.setPlainText(plainText);
    params1.setAddresses(lst);
    params1.setDefaultDomain(defaultDomain);
    params1.setHasAttachment(hasAttachment);
    params1.setTransportId(transportId);
    MessageComposer::PluginEditorCheckBeforeSendParams params2 = params1;
    QVERIFY(params2 == params1);
    QCOMPARE(params2.isHtmlMail(), isHmlMail);
    QCOMPARE(params2.subject(), subject);
    QCOMPARE(params2.plainText(), plainText);
    QCOMPARE(params2.addresses(), lst);
    QCOMPARE(params2.defaultDomain(), defaultDomain);
    QCOMPARE(params2.hasAttachment(), hasAttachment);
    QCOMPARE(params2.transportId(), transportId);
}

void PluginEditorCheckBeforeSendParamsTest::shouldAssignValue()
{
    MessageComposer::PluginEditorCheckBeforeSendParams params1;
    const QString subject(QStringLiteral("foo"));
    const QString plainText(QStringLiteral("bla"));
    const QString defaultDomain(QStringLiteral("bli"));
    bool isHmlMail = true;
    bool hasAttachment = true;
    uint identity = 5;
    int transportId = 6;
    const QStringList lst{ QStringLiteral("foo"), QStringLiteral("foo"), QStringLiteral("foo")};
    params1.setSubject(subject);
    params1.setHtmlMail(isHmlMail);
    params1.setPlainText(plainText);
    params1.setIdentity(identity);
    params1.setAddresses(lst);
    params1.setDefaultDomain(defaultDomain);
    params1.setHasAttachment(hasAttachment);
    params1.setTransportId(transportId);
    QCOMPARE(params1.isHtmlMail(), isHmlMail);
    QCOMPARE(params1.subject(), subject);
    QCOMPARE(params1.identity(), identity);
    QCOMPARE(params1.plainText(), plainText);
    QCOMPARE(params1.addresses(), lst);
    QCOMPARE(params1.defaultDomain(), defaultDomain);
    QCOMPARE(params1.hasAttachment(), hasAttachment);
    QCOMPARE(params1.transportId(), transportId);
}

QTEST_MAIN(PluginEditorCheckBeforeSendParamsTest)
