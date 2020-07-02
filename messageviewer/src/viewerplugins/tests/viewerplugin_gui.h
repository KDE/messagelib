/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEST_VIEWERPLUGIN_GUI_H
#define TEST_VIEWERPLUGIN_GUI_H

#include <QWidget>
namespace MessageViewer {
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

#endif
