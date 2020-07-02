/*
  SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef COMPOSERVIEWBASETEST_H
#define COMPOSERVIEWBASETEST_H

#include <QObject>

class ComposerViewBaseTest : public QObject
{
    Q_OBJECT
public:
    explicit ComposerViewBaseTest(QObject *parent = nullptr);
    ~ComposerViewBaseTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // COMPOSERVIEWBASETEST_H
