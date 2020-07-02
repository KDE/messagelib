/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatesinsertcommandactiontest.h"
#include "templatesinsertcommandaction.h"
#include "templatescommandmenu.h"
#include <QMenu>
#include <QTest>
QTEST_MAIN(TemplatesInsertCommandActionTest)

TemplatesInsertCommandActionTest::TemplatesInsertCommandActionTest(QObject *parent)
    : QObject(parent)
{
}

void TemplatesInsertCommandActionTest::shouldHaveDefaultValue()
{
    TemplateParser::TemplatesInsertCommandAction act;
    QVERIFY(act.menu());
    QVERIFY(!act.menu()->isEmpty());
    TemplateParser::TemplatesCommandMenu *menu = act.findChild<TemplateParser::TemplatesCommandMenu *>(QStringLiteral("templatescommandmenu"));
    QVERIFY(menu);
}
