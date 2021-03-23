/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QWidget>

class ImageScalingTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageScalingTestWidget(QWidget *parent = nullptr);
    ~ImageScalingTestWidget();
};

