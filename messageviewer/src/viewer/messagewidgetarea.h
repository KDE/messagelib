/*
  SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_private_export.h"
#include <QWidget>

namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT MessageWidgetArea : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidgetArea(QWidget *parent = nullptr);
    ~MessageWidgetArea() override;
};
}
