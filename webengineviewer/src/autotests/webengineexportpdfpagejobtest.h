/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class WebEngineExportPdfPageJobTest : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineExportPdfPageJobTest(QObject *parent = nullptr);
    ~WebEngineExportPdfPageJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldBeAbleToExport();
    void shouldEmitSignalFailed();
    void shouldEmitSignalSuccess();
};
