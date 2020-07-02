/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "imagescaling_gui.h"
#include "MessageComposer/ImageScalingWidget"

#include <QHBoxLayout>

#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

ImageScalingTestWidget::ImageScalingTestWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->addWidget(new MessageComposer::ImageScalingWidget(this));
}

ImageScalingTestWidget::~ImageScalingTestWidget()
{
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);
    ImageScalingTestWidget *w = new ImageScalingTestWidget();
    w->resize(800, 600);

    w->show();
    app.exec();
    delete w;
    return 0;
}
