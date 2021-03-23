/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class MailSourceViewTextBrowserWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit MailSourceViewTextBrowserWidgetTest(QObject *parent = nullptr);
    ~MailSourceViewTextBrowserWidgetTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

