/*
  SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#include <QWidget>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <QApplication>
#include <QCommandLineParser>
#include "composer/composerlineedit.h"

using namespace MessageComposer;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);
    QWidget *w = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(w);

    ComposerLineEdit *kale1 = new ComposerLineEdit(0);
    // Add menu for completion
    kale1->enableCompletion(true);
    vbox->addWidget(kale1);
    ComposerLineEdit *kale2 = new ComposerLineEdit(0);
    vbox->addWidget(kale2);
    vbox->addStretch();

    w->resize(400, 400);
    w->show();

    return app.exec();
}
