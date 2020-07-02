/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGEVIEWERRENDERERTEST_H
#define MESSAGEVIEWERRENDERERTEST_H

#include <QObject>

class MessageViewerRendererTest : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerRendererTest(QObject *parent = nullptr);
    ~MessageViewerRendererTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // MESSAGEVIEWERRENDERERTEST_H
