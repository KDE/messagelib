/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORCHECKBEFORESENDPARAMSTEST_H
#define PLUGINEDITORCHECKBEFORESENDPARAMSTEST_H

#include <QObject>

class PluginEditorCheckBeforeSendParamsTest : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSendParamsTest(QObject *parent = nullptr);
    ~PluginEditorCheckBeforeSendParamsTest();

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldBeEqual();
    void shouldAssignValue();
};

#endif // PLUGINEDITORCHECKBEFORESENDPARAMSTEST_H
