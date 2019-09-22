/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "composerattachmentinterfacetest.h"
#include "composer/composerattachmentinterface.h"
#include <QTest>

QTEST_GUILESS_MAIN(ComposerAttachmentInterfaceTest)

ComposerAttachmentInterfaceTest::ComposerAttachmentInterfaceTest(QObject *parent)
    : QObject(parent)
{
}

void ComposerAttachmentInterfaceTest::shouldHaveDefaultValues()
{
    MessageComposer::ComposerAttachmentInterface interface;
    QCOMPARE(interface.count(), 0);
    QVERIFY(interface.namesAndSize().isEmpty());
    QVERIFY(interface.fileNames().isEmpty());
    QVERIFY(interface.names().isEmpty());
}
