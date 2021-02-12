/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATESINSERTCOMMANDACTIONTEST_H
#define TEMPLATESINSERTCOMMANDACTIONTEST_H

#include <QObject>

class TemplatesInsertCommandActionTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplatesInsertCommandActionTest(QObject *parent = nullptr);
    ~TemplatesInsertCommandActionTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // TEMPLATESINSERTCOMMANDACTIONTEST_H
