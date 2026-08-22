/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorcheckbeforesendparamstest.h"

#include <MessageComposer/PluginEditorCheckBeforeSendParams>
#include <QTest>

using namespace Qt::Literals::StringLiterals;
PluginEditorCheckBeforeSendParamsTest::PluginEditorCheckBeforeSendParamsTest(QObject *parent)
    : QObject(parent)
{
}

PluginEditorCheckBeforeSendParamsTest::~PluginEditorCheckBeforeSendParamsTest() = default;

void PluginEditorCheckBeforeSendParamsTest::shouldHaveDefaultValues()
{
    const MessageComposer::PluginEditorCheckBeforeSendParams params;
    QVERIFY(params.subject().isEmpty());
    QVERIFY(params.plainText().isEmpty());
    QCOMPARE(params.identity(), static_cast<uint>(0));
    QCOMPARE(params.transportId(), -1);
    QVERIFY(!params.isHtmlMail());
    QVERIFY(params.bccAddresses().isEmpty());
    QVERIFY(params.ccAddresses().isEmpty());
    QVERIFY(params.toAddresses().isEmpty());
    QVERIFY(params.defaultDomain().isEmpty());
    QVERIFY(!params.hasAttachment());
}

void PluginEditorCheckBeforeSendParamsTest::shouldBeEqual()
{
    MessageComposer::PluginEditorCheckBeforeSendParams params1;
    const QString subject(u"foo"_s);
    const QString plainText(u"bla"_s);
    const QString defaultDomain(u"bli"_s);
    const bool isHmlMail = true;
    const bool hasAttachment = true;
    const int transportId = 5;
    const QString to(u"to"_s);
    const QString cc(u"cc"_s);
    const QString bcc(u"bcc"_s);
    params1.setSubject(subject);
    params1.setHtmlMail(isHmlMail);
    params1.setPlainText(plainText);
    params1.setDefaultDomain(defaultDomain);
    params1.setHasAttachment(hasAttachment);
    params1.setTransportId(transportId);
    params1.setBccAddresses(bcc);
    params1.setCcAddresses(cc);
    params1.setToAddresses(to);

    const MessageComposer::PluginEditorCheckBeforeSendParams params2 = params1;
    QCOMPARE(params2, params1);
    QCOMPARE(params2.isHtmlMail(), isHmlMail);
    QCOMPARE(params2.subject(), subject);
    QCOMPARE(params2.plainText(), plainText);
    QCOMPARE(params2.defaultDomain(), defaultDomain);
    QCOMPARE(params2.hasAttachment(), hasAttachment);
    QCOMPARE(params2.transportId(), transportId);
    QCOMPARE(params2.bccAddresses(), bcc);
    QCOMPARE(params2.ccAddresses(), cc);
    QCOMPARE(params2.toAddresses(), to);
}

void PluginEditorCheckBeforeSendParamsTest::shouldAssignValue()
{
    MessageComposer::PluginEditorCheckBeforeSendParams params1;
    const QString subject(u"foo"_s);
    const QString plainText(u"bla"_s);
    const QString defaultDomain(u"bli"_s);
    const bool isHmlMail = true;
    const bool hasAttachment = true;
    const uint identity = 5;
    const int transportId = 6;
    const QString to(u"to"_s);
    const QString cc(u"cc"_s);
    const QString bcc(u"bcc"_s);

    params1.setSubject(subject);
    params1.setHtmlMail(isHmlMail);
    params1.setPlainText(plainText);
    params1.setIdentity(identity);
    params1.setDefaultDomain(defaultDomain);
    params1.setHasAttachment(hasAttachment);
    params1.setTransportId(transportId);
    params1.setBccAddresses(bcc);
    params1.setCcAddresses(cc);
    params1.setToAddresses(to);

    QCOMPARE(params1.isHtmlMail(), isHmlMail);
    QCOMPARE(params1.subject(), subject);
    QCOMPARE(params1.identity(), identity);
    QCOMPARE(params1.plainText(), plainText);
    QCOMPARE(params1.defaultDomain(), defaultDomain);
    QCOMPARE(params1.hasAttachment(), hasAttachment);
    QCOMPARE(params1.transportId(), transportId);
    QCOMPARE(params1.bccAddresses(), bcc);
    QCOMPARE(params1.ccAddresses(), cc);
    QCOMPARE(params1.toAddresses(), to);
}

QTEST_MAIN(PluginEditorCheckBeforeSendParamsTest)

#include "moc_plugineditorcheckbeforesendparamstest.cpp"
