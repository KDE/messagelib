/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEEXTRACTHTMLELEMENTFROMMAILTEST_H
#define TEMPLATEEXTRACTHTMLELEMENTFROMMAILTEST_H

#include <QObject>

class TemplateExtractHtmlElementFromMailTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateExtractHtmlElementFromMailTest(QObject *parent = nullptr);
    ~TemplateExtractHtmlElementFromMailTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldExtractHtml_data();
    void shouldExtractHtml();
};

#endif // TEMPLATEEXTRACTHTMLELEMENTFROMMAILTEST_H
