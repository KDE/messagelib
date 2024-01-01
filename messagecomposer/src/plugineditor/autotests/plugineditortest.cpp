/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditortest.h"
#include "plugineditor/plugineditor.h"
#include <QTest>
QTEST_GUILESS_MAIN(PluginEditorTest)

PluginEditorTest::PluginEditorTest(QObject *parent)
    : QObject(parent)
{
}

void PluginEditorTest::shouldHaveDefaultValues()
{
    // TODO
}

#include "moc_plugineditortest.cpp"
