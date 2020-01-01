/*
   Copyright (C) 2015-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "viewerplugin_gui.h"
#include "viewerplugins/viewerplugintoolmanager.h"
#include <QStandardPaths>
#include <KActionCollection>

#include <QApplication>
#include <QTextEdit>
#include <QCommandLineParser>
#include <QVBoxLayout>
#include <QMenuBar>

#include <viewerplugins/viewerplugininterface.h>

ViewerPluginTest::ViewerPluginTest(QWidget *parent)
    : QWidget(parent)
{
    QMenuBar *menuBar = new QMenuBar(this);
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(menuBar);
    QTextEdit *textEdit = new QTextEdit(this);
    vbox->addWidget(textEdit);

    QWidget *toolManagerWidget = new QWidget(this);
    vbox->addWidget(toolManagerWidget);
    vbox = new QVBoxLayout;
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    toolManagerWidget->setLayout(vbox);
    MessageViewer::ViewerPluginToolManager *toolManager
        = new MessageViewer::ViewerPluginToolManager(toolManagerWidget, this);
    connect(toolManager, &MessageViewer::ViewerPluginToolManager::activatePlugin, this,
            &ViewerPluginTest::slotActivatePlugin);

    toolManager->setPluginName(QStringLiteral("messageviewer"));
    toolManager->setServiceTypeName(QStringLiteral("MessageViewer/ViewerPlugin"));
    if (!toolManager->initializePluginList()) {
        qDebug() << " Impossible to initialize plugins";
    }
    toolManager->setActionCollection(new KActionCollection(this));
    toolManager->createView();
    QMenu *menu = new QMenu(this);
    menu->setTitle(QStringLiteral("tools"));
    menu->addActions(toolManager->viewerPluginActionList(MessageViewer::ViewerPluginInterface::All));
    menuBar->addMenu(menu);

    menu = new QMenu(this);
    menu->setTitle(QStringLiteral("selected tools"));
    menu->addActions(toolManager->viewerPluginActionList(MessageViewer::ViewerPluginInterface::
                                                         NeedSelection));
    menuBar->addMenu(menu);

    menu = new QMenu(this);
    menu->setTitle(QStringLiteral("message tools"));
    menu->addActions(toolManager->viewerPluginActionList(MessageViewer::ViewerPluginInterface::
                                                         NeedMessage));
    menuBar->addMenu(menu);

    menu = new QMenu(this);
    menu->setTitle(QStringLiteral("message tools and selected tools"));
    MessageViewer::ViewerPluginInterface::SpecificFeatureTypes featureTypes;
    featureTypes |= MessageViewer::ViewerPluginInterface::NeedMessage;
    featureTypes |= MessageViewer::ViewerPluginInterface::NeedSelection;
    featureTypes |= MessageViewer::ViewerPluginInterface::NeedUrl;
    menu->addActions(toolManager->viewerPluginActionList(featureTypes));
    menuBar->addMenu(menu);
}

ViewerPluginTest::~ViewerPluginTest()
{
}

void ViewerPluginTest::slotActivatePlugin(MessageViewer::ViewerPluginInterface *interface)
{
    interface->execute();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    ViewerPluginTest *w = new ViewerPluginTest();
    w->resize(800, 200);
    w->show();
    app.exec();
    delete w;
    return 0;
}
