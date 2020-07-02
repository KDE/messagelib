/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEMPLATEPARSEREXTRACTHTMLINFOTEST_H
#define TEMPLATEPARSEREXTRACTHTMLINFOTEST_H

#include <QObject>

class TemplateParserExtractHtmlInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserExtractHtmlInfoTest(QObject *parent = nullptr);
    ~TemplateParserExtractHtmlInfoTest() = default;
private Q_SLOTS:
    void initTestCase();
    void shouldReturnNullResult();
    void shouldReturnNullButWithTemplate();
};

#endif // TEMPLATEPARSEREXTRACTHTMLINFOTEST_H
