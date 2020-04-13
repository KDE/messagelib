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

#include "developertooldialogtest.h"
#include "widgets/developertooldialog.h"
#include "widgets/developertoolwidget.h"
#include <QTest>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QDialogButtonBox>
QTEST_MAIN(DeveloperToolDialogTest)
DeveloperToolDialogTest::DeveloperToolDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DeveloperToolDialogTest::shouldHaveDefaultValues()
{
   MessageViewer::DeveloperToolDialog d;
   QVBoxLayout *mainLayout = d.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
   QVERIFY(mainLayout);

   MessageViewer::DeveloperToolWidget *mDeveloperToolWidget = d.findChild<MessageViewer::DeveloperToolWidget *>(QStringLiteral("mDeveloperToolWidget"));
   QVERIFY(mDeveloperToolWidget);

   QDialogButtonBox *buttonBox = d.findChild<QDialogButtonBox *>(QStringLiteral("buttonBox"));
   QVERIFY(buttonBox);
}
