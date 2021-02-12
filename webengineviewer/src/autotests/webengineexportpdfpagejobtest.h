/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WEBENGINEEXPORTPDFPAGEJOBTEST_H
#define WEBENGINEEXPORTPDFPAGEJOBTEST_H

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

#endif // WEBENGINEEXPORTPDFPAGEJOBTEST_H
