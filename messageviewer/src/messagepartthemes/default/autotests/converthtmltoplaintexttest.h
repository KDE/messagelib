/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <QObject>

class ConvertHtmlToPlainTextTest : public QObject
{
    Q_OBJECT
public:
    explicit ConvertHtmlToPlainTextTest(QObject *parent = nullptr);
    ~ConvertHtmlToPlainTextTest() override;

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldReturnEmptyStringIfInputTextIsEmpty();
    void shouldReturnNotEmptyStringIfInputTextIsNotEmpty();
    void shouldConvertToPlainText_data();
    void shouldConvertToPlainText();
};
