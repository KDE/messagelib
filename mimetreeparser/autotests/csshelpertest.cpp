/*
  Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "csshelpertest.h"
#include "testcsshelper.h"

#include <QTest>

using namespace MessageViewer;

QTEST_MAIN(CSSHelperTest)

void CSSHelperTest::testCssDefinition()
{
    // load css header
    QFile cssFile(QStringLiteral(MAIL_DATA_DIR) + QStringLiteral("/mailheader.css"));
    QVERIFY(cssFile.open(QIODevice::ReadOnly));

    QImage paintDevice;
    TestCSSHelper cssHelper(&paintDevice);

    QCOMPARE(cssHelper.cssDefinitions(false).trimmed(), QString::fromUtf8(cssFile.readAll().trimmed()));
}
