/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "imagescalingtest.h"

#include "../imagescaling.h"
#include "settings/messagecomposersettings.h"
#include <QStandardPaths>
#include <QTest>

using namespace Qt::Literals::StringLiterals;
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
    QTest::newRow("no rename png when file is empty") << QString() << QString() << QByteArray("image/png") << u"PNG"_s;
    QTest::newRow("no rename jpg when file is empty") << QString() << QString() << QByteArray("image/jpeg") << u"PNG"_s;
    QTest::newRow("no rename to png") << u"foo.jpeg"_s << u"foo.jpeg"_s << QByteArray("image/jpeg") << u"PNG"_s;
    QTest::newRow("no rename to jpeg") << u"foo.png"_s << u"foo.png"_s << QByteArray("image/png") << u"JPG"_s;
    QTest::newRow("rename to jpeg") << u"foo.png"_s << u"foo.jpg"_s << QByteArray("image/mng") << u"JPG"_s;
    QTest::newRow("rename to png") << u"foo.jpg"_s << u"foo.png"_s << QByteArray("image/mng") << u"PNG"_s;
    QTest::newRow("rename jpeg suffix to png") << u"foo.jpeg"_s << u"foo.png"_s << QByteArray("image/mng") << u"PNG"_s;
    QTest::newRow("rename uppercase jpg suffix") << u"foo.JPG"_s << u"foo.png"_s << QByteArray("image/mng") << u"PNG"_s;
    QTest::newRow("rename uppercase png suffix") << u"foo.PNG"_s << u"foo.jpg"_s << QByteArray("image/mng") << u"JPG"_s;
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

    QTest::newRow("no change mimetype when empty") << QByteArray() << QByteArray() << u"PNG"_s;
    QTest::newRow("no change mimetype when empty jpeg") << QByteArray() << QByteArray() << u"JPG"_s;
    QTest::newRow("no change mimetype when jpeg (same)") << QByteArray("image/jpeg") << QByteArray("image/jpeg") << u"JPG"_s;
    QTest::newRow("no change mimetype when jpeg") << QByteArray("image/jpeg") << QByteArray("image/jpeg") << u"PNG"_s;

    QTest::newRow("no change mimetype when png (same)") << QByteArray("image/png") << QByteArray("image/png") << u"JPG"_s;
    QTest::newRow("no change mimetype when png") << QByteArray("image/png") << QByteArray("image/png") << u"PNG"_s;

    QTest::newRow("change mimetype when png") << QByteArray("image/mng") << QByteArray("image/png") << u"PNG"_s;
    QTest::newRow("change mimetype when jpeg") << QByteArray("image/mng") << QByteArray("image/jpeg") << u"JPG"_s;

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
