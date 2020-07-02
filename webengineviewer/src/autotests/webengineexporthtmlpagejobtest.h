/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WEBENGINEEXPORTHTMLPAGEJOBTEST_H
#define WEBENGINEEXPORTHTMLPAGEJOBTEST_H

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

#endif // WEBENGINEEXPORTHTMLPAGEJOBTEST_H
