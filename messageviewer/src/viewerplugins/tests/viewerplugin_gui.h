/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
namespace MessageViewer
{
class ViewerPluginInterface;
}

class ViewerPluginTest : public QWidget
{
    Q_OBJECT
public:
    explicit ViewerPluginTest(QWidget *parent = nullptr);
    ~ViewerPluginTest();

private Q_SLOTS:
    void slotActivatePlugin(MessageViewer::ViewerPluginInterface *interface);
};

