/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "rendertest-smart.h"
using namespace Qt::Literals::StringLiterals;

#include "setupenv.h"
#include "testcsshelper.h"
#include "util.h"

#include <MessageViewer/FileHtmlWriter>
#include <MimeTreeParser/ObjectTreeParser>

#include <KMime/Message>

#include <QDir>
#include <QProcess>
#include <QTest>

using namespace MessageViewer;

void RenderTest::initMain()
{
    Test::setupEnv();
#ifndef Q_OS_WIN
    qputenv("LC_ALL", "en_US.UTF-8");
#endif
}

void RenderTest::testRenderSmart_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + u'/' + file + u".html"_s)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + u'/' + file + u".html"_s) << QString(file + u".out"_s) << u"smart"_s << false
                                                   << QString();
    }
}

void RenderTest::testRenderSmart()
{
    testRender();
}

void RenderTest::testRenderSmartAsync_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + u'/' + file + u".inProgress.html"_s)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + u'/' + file + u".html"_s) << QString(file + u".out"_s) << u"smart"_s << false
                                                   << QString(dir.path() + u'/' + file + u".inProgress.html"_s);
    }
}

void RenderTest::testRenderSmartAsync()
{
    testRender();
}

void RenderTest::testRenderSmartDetails_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/details/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + u".out"_s) << u"smart"_s << true << QString();
    }
}

void RenderTest::testRenderSmartDetails()
{
    testRender();
}

#include "rendertest.cpp"

QTEST_MAIN(RenderTest)

#include "moc_rendertest-smart.cpp"
