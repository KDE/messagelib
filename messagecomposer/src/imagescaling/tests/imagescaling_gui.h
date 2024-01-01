/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QWidget>

class ImageScalingTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageScalingTestWidget(QWidget *parent = nullptr);
    ~ImageScalingTestWidget() override;
};
