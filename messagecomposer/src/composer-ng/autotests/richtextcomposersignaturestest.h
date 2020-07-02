/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RICHTEXTCOMPOSERSIGNATURESTEST_H
#define RICHTEXTCOMPOSERSIGNATURESTEST_H

#include <QObject>

class RichTextComposerSignaturesTest : public QObject
{
    Q_OBJECT
public:
    explicit RichTextComposerSignaturesTest(QObject *parent = nullptr);
    ~RichTextComposerSignaturesTest() = default;
private Q_SLOTS:
    void shouldCleanSignature();
    void shouldReplaceSignatureWhenText();
};

#endif // RICHTEXTCOMPOSERSIGNATURESTEST_H
