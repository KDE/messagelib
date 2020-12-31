/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTCONFIGUREWIDGET_H
#define REMOTECONTENTCONFIGUREWIDGET_H

#include <QWidget>

#include "messageviewer_private_export.h"
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureWidget(QWidget *parent = nullptr);
    ~RemoteContentConfigureWidget() override;
};
}

#endif // REMOTECONTENTCONFIGUREWIDGET_H
