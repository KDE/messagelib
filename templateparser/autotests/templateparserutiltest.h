/*
  SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TEMPLATEPARSERUTILTEST_H
#define TEMPLATEPARSERUTILTEST_H

#include <QObject>

class TemplateParserUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserUtilTest(QObject *parent = nullptr);
    ~TemplateParserUtilTest() = default;

private Q_SLOTS:
    void shouldRemoveSpaceAtBegin_data();
    void shouldRemoveSpaceAtBegin();
};

#endif // TEMPLATEPARSERUTILTEST_H
