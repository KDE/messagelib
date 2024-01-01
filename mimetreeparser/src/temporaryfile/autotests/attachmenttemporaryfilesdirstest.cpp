/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmenttemporaryfilesdirstest.h"
#include "../attachmenttemporaryfilesdirs.h"
#include <QDebug>
#include <QTemporaryDir>
#include <QTest>

using namespace MimeTreeParser;

AttachmentTemporaryFilesDirsTest::AttachmentTemporaryFilesDirsTest(QObject *parent)
    : QObject(parent)
{
}

AttachmentTemporaryFilesDirsTest::~AttachmentTemporaryFilesDirsTest() = default;

void AttachmentTemporaryFilesDirsTest::shouldHaveDefaultValue()
{
    AttachmentTemporaryFilesDirs attachmentDir;
    QVERIFY(attachmentDir.temporaryFiles().isEmpty());
    QVERIFY(attachmentDir.temporaryDirs().isEmpty());
}

void AttachmentTemporaryFilesDirsTest::shouldAddTemporaryFiles()
{
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempFile(QStringLiteral("foo"));
    QCOMPARE(attachmentDir.temporaryFiles().count(), 1);
    attachmentDir.addTempFile(QStringLiteral("foo1"));
    QCOMPARE(attachmentDir.temporaryFiles().count(), 2);
}

void AttachmentTemporaryFilesDirsTest::shouldAddTemporaryDirs()
{
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempDir(QStringLiteral("foo"));
    QCOMPARE(attachmentDir.temporaryDirs().count(), 1);
    attachmentDir.addTempDir(QStringLiteral("foo1"));
    QCOMPARE(attachmentDir.temporaryDirs().count(), 2);
}

void AttachmentTemporaryFilesDirsTest::shouldNotAddSameFiles()
{
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempFile(QStringLiteral("foo"));
    QCOMPARE(attachmentDir.temporaryFiles().count(), 1);
    attachmentDir.addTempFile(QStringLiteral("foo"));
    QCOMPARE(attachmentDir.temporaryFiles().count(), 1);
}

void AttachmentTemporaryFilesDirsTest::shouldNotAddSameDirs()
{
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempDir(QStringLiteral("foo"));
    QCOMPARE(attachmentDir.temporaryDirs().count(), 1);
    attachmentDir.addTempDir(QStringLiteral("foo"));
    QCOMPARE(attachmentDir.temporaryDirs().count(), 1);
}

void AttachmentTemporaryFilesDirsTest::shouldForceRemoveTemporaryDirs()
{
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempDir(QStringLiteral("foo"));
    attachmentDir.addTempDir(QStringLiteral("foo1"));
    QCOMPARE(attachmentDir.temporaryDirs().count(), 2);
    attachmentDir.forceCleanTempFiles();
    QCOMPARE(attachmentDir.temporaryDirs().count(), 0);
    QCOMPARE(attachmentDir.temporaryFiles().count(), 0);
}

void AttachmentTemporaryFilesDirsTest::shouldForceRemoveTemporaryFiles()
{
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempFile(QStringLiteral("foo"));
    attachmentDir.addTempFile(QStringLiteral("foo2"));
    QCOMPARE(attachmentDir.temporaryFiles().count(), 2);
    attachmentDir.forceCleanTempFiles();
    QCOMPARE(attachmentDir.temporaryFiles().count(), 0);
    QCOMPARE(attachmentDir.temporaryDirs().count(), 0);
}

void AttachmentTemporaryFilesDirsTest::shouldCreateDeleteTemporaryFiles()
{
    QTemporaryDir tmpDir;
    QVERIFY(tmpDir.isValid());
    QFile file(tmpDir.path() + QStringLiteral("/foo"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Can open file: " << file.fileName();
        return;
    }
    tmpDir.setAutoRemove(false);
    file.close();
    QVERIFY(file.exists());
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempDir(tmpDir.path());
    attachmentDir.addTempFile(file.fileName());
    QVERIFY(!attachmentDir.temporaryFiles().isEmpty());
    QCOMPARE(attachmentDir.temporaryFiles().constFirst(), file.fileName());
    const QString path = tmpDir.path();
    attachmentDir.forceCleanTempFiles();
    QCOMPARE(attachmentDir.temporaryFiles().count(), 0);
    QCOMPARE(attachmentDir.temporaryDirs().count(), 0);
    QVERIFY(!QDir(path).exists());
}

void AttachmentTemporaryFilesDirsTest::shouldRemoveTemporaryFilesAfterTime()
{
    QTemporaryDir tmpDir;
    QVERIFY(tmpDir.isValid());
    QFile file(tmpDir.path() + QStringLiteral("/foo"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Can open file";
        return;
    }
    tmpDir.setAutoRemove(false);
    file.close();
    QVERIFY(file.exists());
    AttachmentTemporaryFilesDirs attachmentDir;
    attachmentDir.addTempDir(tmpDir.path());
    attachmentDir.addTempFile(file.fileName());
    QVERIFY(!attachmentDir.temporaryFiles().isEmpty());
    QCOMPARE(attachmentDir.temporaryFiles().constFirst(), file.fileName());
    attachmentDir.setDelayRemoveAllInMs(500);
    QTest::qSleep(1000);
    attachmentDir.removeTempFiles();
    const QString path = tmpDir.path();
    attachmentDir.forceCleanTempFiles();
    QCOMPARE(attachmentDir.temporaryFiles().count(), 0);
    QCOMPARE(attachmentDir.temporaryDirs().count(), 0);
    QVERIFY(!QDir(path).exists());
}

QTEST_GUILESS_MAIN(AttachmentTemporaryFilesDirsTest)

#include "moc_attachmenttemporaryfilesdirstest.cpp"
