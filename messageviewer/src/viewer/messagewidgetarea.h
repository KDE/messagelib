/*
  SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#ifndef MESSAGEWIDGETAREA_H
#define MESSAGEWIDGETAREA_H

#include <QWidget>
#include "messageviewer_private_export.h"

namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT MessageWidgetArea : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidgetArea(QWidget *parent = nullptr);
    ~MessageWidgetArea();
};
}
#endif // MESSAGEWIDGETAREA_H
