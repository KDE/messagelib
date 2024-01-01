/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "imagescalingtest.h"
#include "../imagescaling.h"
#include "settings/messagecomposersettings.h"
#include <QStandardPaths>
#include <QTest>

ImageScalingTest::ImageScalingTest(QObject *parent)
    : QObject(parent)
{
}

ImageScalingTest::~ImageScalingTest() = default;

void ImageScalingTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void ImageScalingTest::shouldHaveDefaultValue()
{
    MessageComposer::ImageScaling scaling;
    // Image is empty
    QVERIFY(!scaling.resizeImage());
    QVERIFY(scaling.generateNewName().isEmpty());
}

void ImageScalingTest::shouldHaveRenameFile_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    QTest::addColumn<QByteArray>("format");
    QTest::addColumn<QString>("saveasformat");
    QTest::newRow("no rename png when file is empty") << QString() << QString() << QByteArray("image/png") << QStringLiteral("PNG");
    QTest::newRow("no rename jpg when file is empty") << QString() << QString() << QByteArray("image/jpeg") << QStringLiteral("PNG");
    QTest::newRow("no rename to png") << QStringLiteral("foo.jpeg") << QStringLiteral("foo.jpeg") << QByteArray("image/jpeg") << QStringLiteral("PNG");
    QTest::newRow("no rename to jpeg") << QStringLiteral("foo.png") << QStringLiteral("foo.png") << QByteArray("image/png") << QStringLiteral("JPG");
    QTest::newRow("rename to jpeg") << QStringLiteral("foo.png") << QStringLiteral("foo.jpg") << QByteArray("image/mng") << QStringLiteral("JPG");
    QTest::newRow("rename to png") << QStringLiteral("foo.jpg") << QStringLiteral("foo.png") << QByteArray("image/mng") << QStringLiteral("PNG");
}

void ImageScalingTest::shouldHaveRenameFile()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    QFETCH(QByteArray, format);
    QFETCH(QString, saveasformat);

    MessageComposer::MessageComposerSettings::self()->setWriteFormat(saveasformat);
    MessageComposer::MessageComposerSettings::self()->save();
    MessageComposer::ImageScaling scaling;
    scaling.setName(input);
    scaling.setMimetype(format);
    QCOMPARE(scaling.generateNewName(), output);
}

void ImageScalingTest::shouldHaveChangeMimetype_data()
{
    QTest::addColumn<QByteArray>("initialmimetype");
    QTest::addColumn<QByteArray>("newmimetype");
    QTest::addColumn<QString>("format");

    QTest::newRow("no change mimetype when empty") << QByteArray() << QByteArray() << QStringLiteral("PNG");
    QTest::newRow("no change mimetype when empty jpeg") << QByteArray() << QByteArray() << QStringLiteral("JPG");
    QTest::newRow("no change mimetype when jpeg (same)") << QByteArray("image/jpeg") << QByteArray("image/jpeg") << QStringLiteral("JPG");
    QTest::newRow("no change mimetype when jpeg") << QByteArray("image/jpeg") << QByteArray("image/jpeg") << QStringLiteral("PNG");

    QTest::newRow("no change mimetype when png (same)") << QByteArray("image/png") << QByteArray("image/png") << QStringLiteral("JPG");
    QTest::newRow("no change mimetype when png") << QByteArray("image/png") << QByteArray("image/png") << QStringLiteral("PNG");

    QTest::newRow("change mimetype when png") << QByteArray("image/mng") << QByteArray("image/png") << QStringLiteral("PNG");
    QTest::newRow("change mimetype when jpeg") << QByteArray("image/mng") << QByteArray("image/jpeg") << QStringLiteral("JPG");

    QTest::newRow("When format is not defined but png") << QByteArray("image/png") << QByteArray("image/png") << QString();
    QTest::newRow("When format is not defined but jpeg") << QByteArray("image/jpeg") << QByteArray("image/jpeg") << QString();

    QTest::newRow("When format is not defined but other mimetype (return png)") << QByteArray("image/mng") << QByteArray("image/png") << QString();
}

void ImageScalingTest::shouldHaveChangeMimetype()
{
    QFETCH(QByteArray, initialmimetype);
    QFETCH(QByteArray, newmimetype);
    QFETCH(QString, format);

    MessageComposer::MessageComposerSettings::self()->setWriteFormat(format);
    MessageComposer::MessageComposerSettings::self()->save();
    MessageComposer::ImageScaling scaling;
    scaling.setMimetype(initialmimetype);
    QCOMPARE(scaling.mimetype(), newmimetype);
}

QTEST_MAIN(ImageScalingTest)

#include "moc_imagescalingtest.cpp"
