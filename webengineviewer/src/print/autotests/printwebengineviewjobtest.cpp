/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "printwebengineviewjobtest.h"
#include "../printwebengineviewjob.h"
#include <QTest>
#include <QPageLayout>
#include <QSignalSpy>
#include <QWebEngineView>

PrintWebEngineViewJobTest::PrintWebEngineViewJobTest(QObject *parent)
    : QObject(parent)
{

}

PrintWebEngineViewJobTest::~PrintWebEngineViewJobTest()
{

}

void PrintWebEngineViewJobTest::shouldHaveDefaultValue()
{
    WebEngineViewer::PrintWebEngineViewJob job;
    QVERIFY(!job.engineView());
    QVERIFY(!job.pageLayout().isValid());
}

void PrintWebEngineViewJobTest::shouldFailedWhenWebEngineIsNotDefined()
{
    WebEngineViewer::PrintWebEngineViewJob job;
    QSignalSpy spyFailed(&job, SIGNAL(failed()));
    QSignalSpy spySuccess(&job, SIGNAL(success(QString)));
    job.start();
    QCOMPARE(spyFailed.count(), 1);
    QCOMPARE(spySuccess.count(), 0);
}

void PrintWebEngineViewJobTest::shouldFailedWhenPageLayoutIsNotDefined()
{
    WebEngineViewer::PrintWebEngineViewJob job;
    QSignalSpy spyFailed(&job, SIGNAL(failed()));
    QSignalSpy spySuccess(&job, SIGNAL(success(QString)));
    job.setEngineView(new QWebEngineView());
    job.start();
    QCOMPARE(spyFailed.count(), 1);
    QCOMPARE(spySuccess.count(), 0);

}

QTEST_MAIN(PrintWebEngineViewJobTest)
