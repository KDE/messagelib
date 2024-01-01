/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "templateconfigurewidget_gui.h"
#include "templatesconfiguration.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

TemplateConfigureTestWidget::TemplateConfigureTestWidget(QWidget *parent)
    : QWidget(parent)
{
    auto lay = new QHBoxLayout(this);
    lay->addWidget(new TemplateParser::TemplatesConfiguration(this));
}

TemplateConfigureTestWidget::~TemplateConfigureTestWidget() = default;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    auto w = new TemplateConfigureTestWidget();
    w->resize(800, 600);

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_templateconfigurewidget_gui.cpp"
