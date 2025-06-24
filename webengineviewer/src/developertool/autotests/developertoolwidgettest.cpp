/*
   SPDX-FileCopyrightText: 2020-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "developertoolwidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "developertool/developertoolwidget.h"
#include <QTest>
#include <QVBoxLayout>
#include <QWebEngineSettings>
#include <QWebEngineView>
QTEST_MAIN(DeveloperToolWidgetTest)
DeveloperToolWidgetTest::DeveloperToolWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void DeveloperToolWidgetTest::shouldHaveDefaultValues()
{
    WebEngineViewer::DeveloperToolWidget w;
    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mWebEngineView = w.findChild<QWebEngineView *>(u"mWebEngineView"_s);
    QVERIFY(mWebEngineView);
    mainLayout->addWidget(mWebEngineView);
    auto mEnginePage = w.findChild<QWebEnginePage *>(u"mEnginePage"_s);
    QVERIFY(mEnginePage);
    QCOMPARE(mWebEngineView->page(), mEnginePage);
    QVERIFY(mEnginePage->settings()->testAttribute(QWebEngineSettings::JavascriptEnabled));
}

#include "moc_developertoolwidgettest.cpp"
