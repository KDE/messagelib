/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

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

CustomTemplatesMenuTest::~CustomTemplatesMenuTest() = default;

void CustomTemplatesMenuTest::shouldHaveDefaultValue()
{
    auto collection = new KActionCollection(this);
    TemplateParser::CustomTemplatesMenu templateMenu(nullptr, collection);

    QVERIFY(templateMenu.replyActionMenu());
    QVERIFY(templateMenu.replyAllActionMenu());
    QVERIFY(templateMenu.forwardActionMenu());
}

QTEST_MAIN(CustomTemplatesMenuTest)

#include "moc_customtemplatesmenutest.cpp"
