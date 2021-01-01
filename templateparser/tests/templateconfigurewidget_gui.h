/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef TEST_TEMPLATECONFIGUREWIDGET_GUI_H
#define TEST_TEMPLATECONFIGUREWIDGET_GUI_H

#include <QWidget>

class TemplateConfigureTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateConfigureTestWidget(QWidget *parent = nullptr);
    ~TemplateConfigureTestWidget();
};

#endif
