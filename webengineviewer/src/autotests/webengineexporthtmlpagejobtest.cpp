/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineexporthtmlpagejobtest.h"
#include "../webengineexporthtmlpagejob.h"
#include <QSignalSpy>
#include <QTest>

WebEngineExportHtmlPageJobTest::WebEngineExportHtmlPageJobTest(QObject *parent)
    : QObject(parent)
{
}

WebEngineExportHtmlPageJobTest::~WebEngineExportHtmlPageJobTest()
{
}

void WebEngineExportHtmlPageJobTest::shouldHaveDefaultValue()
{
    WebEngineViewer::WebEngineExportHtmlPageJob job;
    QSignalSpy spyFailed(&job, &WebEngineViewer::WebEngineExportHtmlPageJob::failed);
    QSignalSpy spySuccess(&job, &WebEngineViewer::WebEngineExportHtmlPageJob::success);
    QVERIFY(!job.engineView());
    job.start();
    QCOMPARE(spyFailed.count(), 1);
    QCOMPARE(spySuccess.count(), 0);
}

QTEST_MAIN(WebEngineExportHtmlPageJobTest)
