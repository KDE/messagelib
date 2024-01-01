/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewerrenderertest.h"
#include "viewer/renderer/messageviewerrenderer.h"
#include <QTest>
QTEST_MAIN(MessageViewerRendererTest)

MessageViewerRendererTest::MessageViewerRendererTest(QObject *parent)
    : QObject(parent)
{
}

void MessageViewerRendererTest::shouldHaveDefaultValue()
{
    MessageViewer::MessageViewerRenderer w;
    QVERIFY(!w.currentWidget());
    QVERIFY(!w.cssHelper());
}

#include "moc_messageviewerrenderertest.cpp"
