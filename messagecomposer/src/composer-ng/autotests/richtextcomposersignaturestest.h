/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class RichTextComposerSignaturesTest : public QObject
{
    Q_OBJECT
public:
    explicit RichTextComposerSignaturesTest(QObject *parent = nullptr);
    ~RichTextComposerSignaturesTest() override = default;
private Q_SLOTS:
    void shouldCleanSignature();
    void shouldReplaceSignatureWhenText();
};
