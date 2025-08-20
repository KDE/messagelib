/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "rendertest-headeronly.h"
using namespace Qt::Literals::StringLiterals;

#include "setupenv.h"
#include "testcsshelper.h"
#include "util.h"

#include <MessageViewer/FileHtmlWriter>
#include <MimeTreeParser/ObjectTreeParser>

#include <KMime/Message>

#include <QDir>
#include <QTest>

using namespace MessageViewer;

void RenderTest::initMain()
{
    Test::setupEnv();
#ifndef Q_OS_WIN
    qputenv("LC_ALL", "en_US.UTF-8");
#endif
}

void RenderTest::testRenderHeaderOnly_data()
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
        QString fname = dir.path() + QLatin1StringView("/headeronly/") + file + u".html"_s;
        if (!QFile::exists(fname)) {
            fname = dir.path() + u'/' + file + QLatin1StringView(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + u".out"_s) << u"headeronly"_s << false << QString();
    }
}

void RenderTest::testRenderHeaderOnly()
{
    testRender();
}

#include "rendertest.cpp"

QTEST_MAIN(RenderTest)

#include "moc_rendertest-headeronly.cpp"
