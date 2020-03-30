/*
   Copyright (C) 2020 Laurent Montel <montel@kde.org>

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
    QWebEngineView *webEngine = new QWebEngineView;

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
    QWebEngineView *webEngine = new QWebEngineView;
    job.setEngineView(webEngine);

    job.start();
    QVERIFY(spySuccess.wait());

    QCOMPARE(spyFailed.count(), 0);
    QCOMPARE(spySuccess.count(), 1);
    delete webEngine;
}
