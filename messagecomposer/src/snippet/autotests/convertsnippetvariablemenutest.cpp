/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablemenutest.h"
#include "snippet/convertsnippetvariablemenu.h"
#include <QTest>
QTEST_MAIN(ConvertSnippetVariableMenuTest)

ConvertSnippetVariableMenuTest::ConvertSnippetVariableMenuTest(QObject *parent)
    : QObject(parent)
{
}

void ConvertSnippetVariableMenuTest::shouldHaveDefaultValues()
{
    MessageComposer::ConvertSnippetVariableMenu w(true, nullptr);
    QVERIFY(w.menu());
}

#include "moc_convertsnippetvariablemenutest.cpp"
