/*
  SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef TEST_TEMPLATEPARSEREMAILREQUESTER_GUI_H
#define TEST_TEMPLATEPARSEREMAILREQUESTER_GUI_H

#include <QWidget>

class TemplateParserEmailRequesterTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateParserEmailRequesterTestWidget(QWidget *parent = nullptr);
    ~TemplateParserEmailRequesterTestWidget();
};

#endif
