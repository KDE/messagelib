/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "createdatabasefilejobtest.h"
#include "../createdatabasefilejob.h"
#include "../createphishingurldatabasejob.h"
#include "../localdatabasefile.h"
#include <QStandardPaths>

#include <QSignalSpy>
#include <QTest>
#include <QDebug>

QByteArray readJsonFile(const QString &jsonFile)
{
    QFile file(QLatin1String(CHECKPHISHINGURL_DATA_DIR) + QLatin1Char('/') + jsonFile);
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    const QByteArray data = file.readAll();
    Q_ASSERT(!data.isEmpty());
    return data;
}

CreateDatabaseFileJobTest::CreateDatabaseFileJobTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

CreateDatabaseFileJobTest::~CreateDatabaseFileJobTest()
{

}

void CreateDatabaseFileJobTest::shouldHaveDefaultValue()
{
    WebEngineViewer::CreateDatabaseFileJob job;
    QVERIFY(!job.canStart());
}

void CreateDatabaseFileJobTest::shouldCreateFile()
{
    QString filename = QStringLiteral("test2.json");
    const QByteArray ba = readJsonFile(filename);
    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    QSignalSpy spy1(&job, SIGNAL(finished(WebEngineViewer::UpdateDataBaseInfo,WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)));
    job.parseResult(ba);
    QCOMPARE(spy1.count(), 1);
    const WebEngineViewer::UpdateDataBaseInfo info = spy1.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>();
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName = QLatin1String(CHECKPHISHINGURL_DATA_DIR) + QStringLiteral("/test.db");
    qDebug()<<" new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);
    databasejob.setUpdateDataBaseInfo(info);
    databasejob.start();

    WebEngineViewer::LocalDataBaseFile newFile(createDataBaseName);
    QVERIFY(newFile.isValid());

}

QTEST_MAIN(CreateDatabaseFileJobTest)
