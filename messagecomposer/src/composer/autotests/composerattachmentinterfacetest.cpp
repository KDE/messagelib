/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

#include "moc_composerattachmentinterfacetest.cpp"
