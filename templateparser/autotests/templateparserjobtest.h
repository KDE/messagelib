/*
   Copyright (C) 2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TEMPLATEPARSERJOBTEST_H
#define TEMPLATEPARSERJOBTEST_H

#include <QObject>

class TemplateParserJobTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserJobTest(QObject *parent = nullptr);
    ~TemplateParserJobTest() = default;
private Q_SLOTS:
    /**
     * checks whether text/plain only mails are converted to a valid HTML
     */

//    void test_convertedHtml();
//    void test_convertedHtml_data();

//    void test_replyPlain();
//    void test_replyPlain_data();

    /**
     * Tests whether templates are returning required body or not
     */
    void test_processWithTemplatesForBody();
    void test_processWithTemplatesForBody_data();

    void test_processWithTemplatesForContent();
    void test_processWithTemplatesForContent_data();

    void test_processWithTemplatesForContentOtherTimeZone();
    void test_processWithTemplatesForContentOtherTimeZone_data();

};

#endif // TEMPLATEPARSERJOBTEST_H
