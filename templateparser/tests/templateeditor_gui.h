/*
  SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef TEST_TEMPLATEEDITOR_GUI_H
#define TEST_TEMPLATEEDITOR_GUI_H

#include <QWidget>

class TemplateEditorTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateEditorTestWidget(QWidget *parent = nullptr);
    ~TemplateEditorTestWidget();
};

#endif
