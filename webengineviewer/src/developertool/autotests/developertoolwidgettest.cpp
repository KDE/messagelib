/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "developertoolwidgettest.h"
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
    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mWebEngineView = w.findChild<QWebEngineView *>(QStringLiteral("mWebEngineView"));
    QVERIFY(mWebEngineView);
    mainLayout->addWidget(mWebEngineView);
    auto mEnginePage = w.findChild<QWebEnginePage *>(QStringLiteral("mEnginePage"));
    QVERIFY(mEnginePage);
    QCOMPARE(mWebEngineView->page(), mEnginePage);
    QVERIFY(mEnginePage->settings()->testAttribute(QWebEngineSettings::JavascriptEnabled));
}

#include "moc_developertoolwidgettest.cpp"
