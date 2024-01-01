/*
  SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QWidget>

class TemplateParserEmailRequesterTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TemplateParserEmailRequesterTestWidget(QWidget *parent = nullptr);
    ~TemplateParserEmailRequesterTestWidget() override;
};
