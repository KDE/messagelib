/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimrulewidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimmanagerulescombobox.h"
#include "dkim-verify/dkimrulewidget.h"
#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTest>

QTEST_MAIN(DKIMRuleWidgetTest)
DKIMRuleWidgetTest::DKIMRuleWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMRuleWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMRuleWidget w;
    QVERIFY(!w.rule().isValid());

    auto layout = w.findChild<QFormLayout *>(u"layout"_s);
    QCOMPARE(layout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mEnabled = w.findChild<QCheckBox *>(u"enabled"_s);
    QVERIFY(mEnabled);
    QVERIFY(!mEnabled->text().isEmpty());
    QVERIFY(mEnabled->isChecked());

    auto mDomain = w.findChild<QLineEdit *>(u"domain"_s);
    QVERIFY(mDomain);
    QVERIFY(mDomain->text().isEmpty());
    QVERIFY(mDomain->isClearButtonEnabled());

    auto mListId = w.findChild<QLineEdit *>(u"listid"_s);
    QVERIFY(mListId);
    QVERIFY(mListId->text().isEmpty());
    QVERIFY(mListId->isClearButtonEnabled());

    auto mFrom = w.findChild<QLineEdit *>(u"from"_s);
    QVERIFY(mFrom);
    QVERIFY(mFrom->text().isEmpty());
    QVERIFY(mFrom->isClearButtonEnabled());

    auto mSignatureDomainIdentifier = w.findChild<QLineEdit *>(u"signaturedomainidentifier"_s);
    QVERIFY(mSignatureDomainIdentifier);
    QVERIFY(mSignatureDomainIdentifier->text().isEmpty());
    QVERIFY(mSignatureDomainIdentifier->isClearButtonEnabled());

    auto mRuleType = w.findChild<MessageViewer::DKIMManageRulesComboBox *>(u"ruletype"_s);
    QVERIFY(mRuleType);

    auto mPriority = w.findChild<QSpinBox *>(u"priority"_s);
    QVERIFY(mPriority);
    QCOMPARE(mPriority->value(), 1000);
}

#include "moc_dkimrulewidgettest.cpp"
