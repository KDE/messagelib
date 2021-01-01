/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "customtemplatesmenutest.h"
#include "customtemplatesmenu.h"
#include <KActionCollection>
#include <QTest>

CustomTemplatesMenuTest::CustomTemplatesMenuTest(QObject *parent)
    : QObject(parent)
{
}

CustomTemplatesMenuTest::~CustomTemplatesMenuTest()
{
}

void CustomTemplatesMenuTest::shouldHaveDefaultValue()
{
    auto collection = new KActionCollection(this);
    TemplateParser::CustomTemplatesMenu templateMenu(nullptr, collection);

    QVERIFY(templateMenu.replyActionMenu());
    QVERIFY(templateMenu.replyAllActionMenu());
    QVERIFY(templateMenu.forwardActionMenu());
}

QTEST_MAIN(CustomTemplatesMenuTest)
