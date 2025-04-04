/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class WebHitTestResultTest : public QObject
{
    Q_OBJECT
public:
    explicit WebHitTestResultTest(QObject *parent = nullptr);
    ~WebHitTestResultTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldAssignPosAndUrl();
    void shouldAssignFromQVariant();
    void shouldCopyWebHitTestResult();
};
