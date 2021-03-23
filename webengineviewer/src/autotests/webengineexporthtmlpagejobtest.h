/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class WebEngineExportHtmlPageJobTest : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineExportHtmlPageJobTest(QObject *parent = nullptr);
    ~WebEngineExportHtmlPageJobTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

