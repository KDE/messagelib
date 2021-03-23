/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class AttachmentUpdateJobTest : public QObject
{
    Q_OBJECT
public:
    explicit AttachmentUpdateJobTest(QObject *parent = nullptr);
    ~AttachmentUpdateJobTest();

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldUpdateAttachment();
    void shouldHaveSameNameDescriptionAfterUpdate();
    void shouldHaveSameCryptoSignStatusAfterUpdate();
    void shouldHaveSameEncodingAfterUpdate();
    void shouldHaveSameMimetypeAfterUpdate();
    void shouldNotUpdateWhenUrlIsEmpty();
    void shouldHaveSameInlineStatus();
};

