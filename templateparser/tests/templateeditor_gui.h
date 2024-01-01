/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QWidget>

class TemplateEditorTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateEditorTestWidget(QWidget *parent = nullptr);
    ~TemplateEditorTestWidget() override;
};
