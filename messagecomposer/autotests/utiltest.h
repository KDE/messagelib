/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef UTILTEST_H
#define UTILTEST_H

#include <QObject>

class UtilTest : public QObject
{
    Q_OBJECT
public:
    explicit UtilTest(QObject *parent = nullptr);
private Q_SLOTS:
    void testSelectCharset();
    void shouldTestHasMissingAttachment();
    void shouldTestHasMissingAttachment_data();
};

#endif
