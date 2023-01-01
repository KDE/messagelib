/*
  SPDX-FileCopyrightText: 2016-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <QObject>

class SearchCollectionIndexingWarningTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchCollectionIndexingWarningTest(QObject *parent = nullptr);
    ~SearchCollectionIndexingWarningTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};
