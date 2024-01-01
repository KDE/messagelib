/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#include <QVBoxLayout>
#include <QWidget>

#include "composer/composerlineedit.h"
#include <QApplication>
#include <QCommandLineParser>

using namespace MessageComposer;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);
    auto w = new QWidget;
    auto vbox = new QVBoxLayout(w);

    auto kale1 = new ComposerLineEdit(false);
    // Add menu for completion
    kale1->enableCompletion(true);
    vbox->addWidget(kale1);
    auto kale2 = new ComposerLineEdit(false);
    vbox->addWidget(kale2);
    vbox->addStretch();

    w->resize(400, 400);
    w->show();

    return app.exec();
}
