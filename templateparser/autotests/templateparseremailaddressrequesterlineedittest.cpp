/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparseremailaddressrequesterlineedittest.h"
#include "templateparseremailaddressrequesterlineedit.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSignalSpy>
#include <QTest>

TemplateParserEmailAddressRequesterLineEditTest::TemplateParserEmailAddressRequesterLineEditTest(QObject *parent)
    : QObject(parent)
{
}

void TemplateParserEmailAddressRequesterLineEditTest::shouldHaveDefaultValue()
{
    TemplateParser::TemplateParserEmailAddressRequesterLineEdit w;

    auto mainLayout = w.findChild<QHBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mLineEdit = w.findChild<QLineEdit *>(QStringLiteral("lineedit"));
    QVERIFY(mLineEdit);
    QVERIFY(mLineEdit->text().isEmpty());
}

void TemplateParserEmailAddressRequesterLineEditTest::shouldAssignValue()
{
    TemplateParser::TemplateParserEmailAddressRequesterLineEdit w;
    auto mLineEdit = w.findChild<QLineEdit *>(QStringLiteral("lineedit"));
    const QString str{QStringLiteral("foo")};
    w.setText(str);
    QCOMPARE(w.text(), str);
    QCOMPARE(mLineEdit->text(), str);
}

void TemplateParserEmailAddressRequesterLineEditTest::shouldClearValue()
{
    TemplateParser::TemplateParserEmailAddressRequesterLineEdit w;
    auto mLineEdit = w.findChild<QLineEdit *>(QStringLiteral("lineedit"));
    const QString str{QStringLiteral("foo")};
    w.setText(str);
    QCOMPARE(w.text(), str);
    w.clear();
    QVERIFY(w.text().isEmpty());
    QVERIFY(mLineEdit->text().isEmpty());
}

void TemplateParserEmailAddressRequesterLineEditTest::shouldEmitSignal()
{
    TemplateParser::TemplateParserEmailAddressRequesterLineEdit w;
    QSignalSpy spy(&w, &TemplateParser::TemplateParserEmailAddressRequesterBase::textChanged);
    w.setText(QStringLiteral("foo"));
    QCOMPARE(spy.count(), 1);
    w.clear();
    QCOMPARE(spy.count(), 2);
    w.setText(QStringLiteral("foo"));
    QCOMPARE(spy.count(), 3);
}

QTEST_MAIN(TemplateParserEmailAddressRequesterLineEditTest)

#include "moc_templateparseremailaddressrequesterlineedittest.cpp"
