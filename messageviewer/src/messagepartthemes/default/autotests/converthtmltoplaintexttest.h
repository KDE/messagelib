/*
  SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#ifndef CONVERTHTMLTOPLAINTEXTTEST_H
#define CONVERTHTMLTOPLAINTEXTTEST_H

#include <QObject>

class ConvertHtmlToPlainTextTest : public QObject
{
    Q_OBJECT
public:
    explicit ConvertHtmlToPlainTextTest(QObject *parent = nullptr);
    ~ConvertHtmlToPlainTextTest();

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldReturnEmptyStringIfInputTextIsEmpty();
    void shouldReturnNotEmptyStringIfInputTextIsNotEmpty();
    void shouldConvertToPlainText_data();
    void shouldConvertToPlainText();
};

#endif // CONVERTHTMLTOPLAINTEXTTEST_H
