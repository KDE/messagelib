/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "textparttest.h"
#include <../src/part/textpart.h>
#include <QTest>
TextPartTest::TextPartTest(QObject *parent)
    : QObject(parent)
{
}

TextPartTest::~TextPartTest()
{
}

void TextPartTest::shouldHaveDefaultValue()
{
    MessageComposer::TextPart textpart;
    QVERIFY(textpart.warnBadCharset());
    QVERIFY(textpart.isWordWrappingEnabled());
    QVERIFY(!textpart.hasEmbeddedImages());
    QVERIFY(!textpart.isHtmlUsed());
}

QTEST_APPLESS_MAIN(TextPartTest)
