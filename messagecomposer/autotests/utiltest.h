/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class UtilTest : public QObject
{
    Q_OBJECT
public:
    explicit UtilTest(QObject *parent = nullptr);
private Q_SLOTS:
    void shouldTestHasMissingAttachment();
    void shouldTestHasMissingAttachment_data();
};
