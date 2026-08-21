/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ItipJobTest : public QObject
{
    Q_OBJECT
public:
    explicit ItipJobTest(QObject *parent = nullptr);

private Q_SLOTS:
    void testInvitationWithAttachment();
    void testInvitationWithoutAttachment();
    void testOutlookInvitationWithAttachment();
    void testOutlookInvitationWithoutAttachment();
};
