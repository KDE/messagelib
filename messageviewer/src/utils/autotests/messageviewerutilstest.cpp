/*
  Copyright (C) 2018-2019 Montel Laurent <montel@kde.org>

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

#include "messageviewerutilstest.h"
#include "utils/messageviewerutil.h"
#include <QTest>
QTEST_GUILESS_MAIN(MessageViewerUtilsTest)

MessageViewerUtilsTest::MessageViewerUtilsTest(QObject *parent)
    : QObject(parent)
{
}

void MessageViewerUtilsTest::shouldExcludeHeader_data()
{
    QTest::addColumn<QString>("header");
    QTest::addColumn<bool>("exclude");
    QTest::newRow("emptylist") << QString() << false;
    QTest::newRow("REFRESH1") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\'REFRESH\'></head>") << true;
    QTest::newRow("REFRESH2") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\"REFRESH\"></head>") << true;
    QTest::newRow("REFRESH3") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\"refresh\"></head>") << true;
    QTest::newRow("REFRESH4") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\"&#82;EFRESH\"></head>") << true;
    QTest::newRow("REFRESH5") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\'&#82;EFRESH\'></head>") << true;
    QTest::newRow("REFRESH6") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv= \"REFRESH\"></head>") << true;
}

void MessageViewerUtilsTest::shouldExcludeHeader()
{
    QFETCH(QString, header);
    QFETCH(bool, exclude);
    QCOMPARE(MessageViewer::Util::excludeExtraHeader(header), exclude);
}
