/*
  SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef WEBENGINEEMBEDPARTTEST_H
#define WEBENGINEEMBEDPARTTEST_H

#include <QObject>

class WebEngineEmbedPartTest : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineEmbedPartTest(QObject *parent = nullptr);
    ~WebEngineEmbedPartTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldClearValue();
    void shouldAddValues();
    void shouldAddTwoIdenticalValues();
};

#endif // WEBENGINEEMBEDPARTTEST_H
