/*
  SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef CUSTOMTEMPLATESMENUTEST_H
#define CUSTOMTEMPLATESMENUTEST_H

#include <QObject>

class CustomTemplatesMenuTest : public QObject
{
    Q_OBJECT
public:
    explicit CustomTemplatesMenuTest(QObject *parent = nullptr);
    ~CustomTemplatesMenuTest();

private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // CUSTOMTEMPLATESMENUTEST_H
