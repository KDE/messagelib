/*
   Copyright (C) 2016-2019 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef RICHTEXTCOMPOSERNGTEST_H
#define RICHTEXTCOMPOSERNGTEST_H

#include <QObject>

class RichTextComposerNgTest : public QObject
{
    Q_OBJECT
public:
    explicit RichTextComposerNgTest(QObject *parent = nullptr);
    ~RichTextComposerNgTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldForceAutoCorrection_data();
    void shouldForceAutoCorrection();

    void shouldForceAutoCorrectionWithSelection_data();
    void shouldForceAutoCorrectionWithSelection();

    void shouldNotChangeSignatureWhenOriginalAndNewSignatureAreSame();

    void shouldAddSignature_data();
    void shouldAddSignature();

    void shouldAddSpecificSignature_data();
    void shouldAddSpecificSignature();

    void shouldReplaceSignature_data();
    void shouldReplaceSignature();

    void shouldLoadSignatureFromFile_data();
    void shouldLoadSignatureFromFile();

    void shouldLoadSignatureFromCommand_data();
    void shouldLoadSignatureFromCommand();
};

#endif // RICHTEXTCOMPOSERNGTEST_H
