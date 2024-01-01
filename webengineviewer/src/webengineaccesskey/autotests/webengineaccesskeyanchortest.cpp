/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineaccesskeyanchortest.h"
#include "../webengineaccesskeyanchor.h"
#include <QTest>

WebEngineAccessKeyAnchorTest::WebEngineAccessKeyAnchorTest(QObject *parent)
    : QObject(parent)
{
}

WebEngineAccessKeyAnchorTest::~WebEngineAccessKeyAnchorTest() = default;

void WebEngineAccessKeyAnchorTest::shouldReturnEmptyAccessKeyAnchor()
{
    QVariant var;
    WebEngineViewer::WebEngineAccessKeyAnchor accessKeyAnchor(var);
    QVERIFY(accessKeyAnchor.href().isEmpty());
    QVERIFY(accessKeyAnchor.accessKey().isEmpty());
    QVERIFY(accessKeyAnchor.boundingRect().isEmpty());
    QVERIFY(accessKeyAnchor.tagName().isEmpty());
    QVERIFY(accessKeyAnchor.target().isEmpty());
    QVERIFY(accessKeyAnchor.innerText().isEmpty());
}

QTEST_APPLESS_MAIN(WebEngineAccessKeyAnchorTest)

#include "moc_webengineaccesskeyanchortest.cpp"
