/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineexportpdfpagejobtest.h"
#include "webengineexportpdfpagejob.h"
#include <QSignalSpy>
#include <QTest>
#include <QWebEngineView>
QTEST_MAIN(WebEngineExportPdfPageJobTest)
WebEngineExportPdfPageJobTest::WebEngineExportPdfPageJobTest(QObject *parent)
    : QObject(parent)
{
}

void WebEngineExportPdfPageJobTest::shouldHaveDefaultValue()
{
    WebEngineViewer::WebEngineExportPdfPageJob job;
    QVERIFY(!job.engineView());
    QVERIFY(job.pdfPath().isEmpty());
    QVERIFY(!job.canStart());
}

void WebEngineExportPdfPageJobTest::shouldBeAbleToExport()
{
    WebEngineViewer::WebEngineExportPdfPageJob job;
    QVERIFY(!job.canStart());
    job.setPdfPath(QStringLiteral("foo"));
    QVERIFY(!job.canStart());
    auto webEngine = new QWebEngineView;

    job.setEngineView(webEngine);
    QVERIFY(job.canStart());
    delete webEngine;
}

void WebEngineExportPdfPageJobTest::shouldEmitSignalFailed()
{
    WebEngineViewer::WebEngineExportPdfPageJob job;
    QSignalSpy spyFailed(&job, &WebEngineViewer::WebEngineExportPdfPageJob::exportPdfFailed);
    QSignalSpy spySuccess(&job, &WebEngineViewer::WebEngineExportPdfPageJob::exportToPdfSuccess);
    QVERIFY(!job.engineView());
    job.start();
    QCOMPARE(spyFailed.count(), 1);
    QCOMPARE(spySuccess.count(), 0);
}

void WebEngineExportPdfPageJobTest::shouldEmitSignalSuccess()
{
    WebEngineViewer::WebEngineExportPdfPageJob job;
    QSignalSpy spyFailed(&job, &WebEngineViewer::WebEngineExportPdfPageJob::exportPdfFailed);
    QSignalSpy spySuccess(&job, &WebEngineViewer::WebEngineExportPdfPageJob::exportToPdfSuccess);
    job.setPdfPath(QDir::tempPath() + QStringLiteral("/test-webengine-export-test.pdf"));
    auto webEngine = new QWebEngineView;
    job.setEngineView(webEngine);

    job.start();
    QVERIFY(spySuccess.wait());

    QCOMPARE(spyFailed.count(), 0);
    QCOMPARE(spySuccess.count(), 1);
    delete webEngine;
}
