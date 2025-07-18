/*
   SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "viewerplugin_gui.h"
using namespace Qt::Literals::StringLiterals;

#include "viewerplugins/viewerplugintoolmanager.h"
#include <KActionCollection>
#include <QStandardPaths>

#include <QApplication>
#include <QCommandLineParser>
#include <QMenuBar>
#include <QTextEdit>
#include <QVBoxLayout>

#include "viewerplugins/viewerplugininterface.h"

ViewerPluginTest::ViewerPluginTest(QWidget *parent)
    : QWidget(parent)
{
    auto menuBar = new QMenuBar(this);
    auto vbox = new QVBoxLayout(this);
    vbox->addWidget(menuBar);
    auto textEdit = new QTextEdit(this);
    vbox->addWidget(textEdit);

    auto toolManagerWidget = new QWidget(this);
    vbox->addWidget(toolManagerWidget);
    vbox = new QVBoxLayout;
    vbox->setContentsMargins({});
    vbox->setSpacing(0);
    toolManagerWidget->setLayout(vbox);
    auto toolManager = new MessageViewer::ViewerPluginToolManager(toolManagerWidget, this);
    connect(toolManager, &MessageViewer::ViewerPluginToolManager::activatePlugin, this, &ViewerPluginTest::slotActivatePlugin);

    toolManager->setPluginName(u"messageviewer"_s);
    toolManager->setPluginDirectory(u"pim6/messageviewer/viewerplugin"_s);
    if (!toolManager->initializePluginList()) {
        qDebug() << " Impossible to initialize plugins";
    }
    toolManager->setActionCollection(new KActionCollection(this));
    toolManager->createView();
    auto menu = new QMenu(this);
    menu->setTitle(u"tools"_s);
    menu->addActions(toolManager->viewerPluginActionList(MessageViewer::ViewerPluginInterface::All));
    menuBar->addMenu(menu);

    menu = new QMenu(this);
    menu->setTitle(u"selected tools"_s);
    menu->addActions(toolManager->viewerPluginActionList(MessageViewer::ViewerPluginInterface::NeedSelection));
    menuBar->addMenu(menu);

    menu = new QMenu(this);
    menu->setTitle(u"message tools"_s);
    menu->addActions(toolManager->viewerPluginActionList(MessageViewer::ViewerPluginInterface::NeedMessage));
    menuBar->addMenu(menu);

    menu = new QMenu(this);
    menu->setTitle(u"message tools and selected tools"_s);
    MessageViewer::ViewerPluginInterface::SpecificFeatureTypes featureTypes;
    featureTypes |= MessageViewer::ViewerPluginInterface::NeedMessage;
    featureTypes |= MessageViewer::ViewerPluginInterface::NeedSelection;
    featureTypes |= MessageViewer::ViewerPluginInterface::NeedUrl;
    menu->addActions(toolManager->viewerPluginActionList(featureTypes));
    menuBar->addMenu(menu);
}

ViewerPluginTest::~ViewerPluginTest() = default;

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

    auto w = new ViewerPluginTest();
    w->resize(800, 200);
    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_viewerplugin_gui.cpp"
