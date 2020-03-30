/*
  Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MESSAGEVIEWERUTILSTEST_H
#define MESSAGEVIEWERUTILSTEST_H

#include <QObject>

class MessageViewerUtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerUtilsTest(QObject *parent = nullptr);
    ~MessageViewerUtilsTest() = default;
private Q_SLOTS:
    void shouldExcludeHeader_data();
    void shouldExcludeHeader();

    void shouldContainsExternalReferences_data();
    void shouldContainsExternalReferences();

    void shouldExtractHtml();
    void shouldExtractHtml_data();

    void shouldUseCorrectCodec();
    void shouldUseCorrectCodec_data();
};

#endif // MESSAGEVIEWERUTILSTEST_H
