/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineexportpdfpagejobtest.h"
using namespace Qt::Literals::StringLiterals;

#include "webengineexportpdfpagejob.h"
#include <QDialog>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <QTimer>
#include <QWebEngineView>
QTEST_MAIN(WebEngineExportPdfPageJobTest)
WebEngineExportPdfPageJobTest::WebEngineExportPdfPageJobTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
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
    job.setPdfPath(u"foo"_s);
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
#ifdef Q_OS_WIN
    QSKIP("QPageSetupDialog: Cannot be used on non-native printers");
#endif

    WebEngineViewer::WebEngineExportPdfPageJob job;
    QSignalSpy spyFailed(&job, &WebEngineViewer::WebEngineExportPdfPageJob::exportPdfFailed);
    QSignalSpy spySuccess(&job, &WebEngineViewer::WebEngineExportPdfPageJob::exportToPdfSuccess);
    job.setPdfPath(QDir::tempPath() + u"/test-webengine-export-test.pdf"_s);
    auto webEngine = new QWebEngineView;
    job.setEngineView(webEngine);

    // close the preview dialog start() will show
    QTimer::singleShot(500, this, [webEngine]() {
        auto previewDialog = webEngine->findChild<QDialog *>();
        QTest::keyClick(previewDialog, Qt::Key_Return);
    });

    job.start();
    QVERIFY(spySuccess.wait());

    QCOMPARE(spyFailed.count(), 0);
    QCOMPARE(spySuccess.count(), 1);
    delete webEngine;
}

#include "moc_webengineexportpdfpagejobtest.cpp"
