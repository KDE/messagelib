/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatesinsertcommandactiontest.h"
using namespace Qt::Literals::StringLiterals;

#include "templatescommandmenu.h"
#include "templatesinsertcommandaction.h"
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
    auto menu = act.findChild<TemplateParser::TemplatesCommandMenu *>(u"templatescommandmenu"_s);
    QVERIFY(menu);
}

#include "moc_templatesinsertcommandactiontest.cpp"
