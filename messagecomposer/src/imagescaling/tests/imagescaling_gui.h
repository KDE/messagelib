/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TEST_IMAGESCALING_GUI_H
#define TEST_IMAGESCALING_GUI_H

#include <QWidget>

class ImageScalingTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageScalingTestWidget(QWidget *parent = nullptr);
    ~ImageScalingTestWidget();
};

#endif
