/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatesinsertcommandpushbuttontest.h"
#include "templatesinsertcommandpushbutton.h"
#include "templatescommandmenu.h"

#include <QTest>
#include <QMenu>

QTEST_MAIN(TemplatesInsertCommandPushButtonTest)

TemplatesInsertCommandPushButtonTest::TemplatesInsertCommandPushButtonTest(QObject *parent)
    : QObject(parent)
{
}

void TemplatesInsertCommandPushButtonTest::shouldHaveDefaultValue()
{
    TemplateParser::TemplatesInsertCommandPushButton act(nullptr);
    QVERIFY(act.menu());
    QVERIFY(!act.menu()->isEmpty());
    QCOMPARE(act.type(), TemplateParser::TemplatesCommandMenu::Default);
    TemplateParser::TemplatesCommandMenu *menu = act.findChild<TemplateParser::TemplatesCommandMenu *>(QStringLiteral("templatescommandmenu"));
    QVERIFY(menu);
}
