/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatesinsertcommandpushbuttontest.h"
#include "templatescommandmenu.h"
#include "templatesinsertcommandpushbutton.h"

#include <QMenu>
#include <QTest>

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
    auto menu = act.findChild<TemplateParser::TemplatesCommandMenu *>(QStringLiteral("templatescommandmenu"));
    QVERIFY(menu);
}

#include "moc_templatesinsertcommandpushbuttontest.cpp"
