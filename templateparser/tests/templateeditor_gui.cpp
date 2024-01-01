/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "templateeditor_gui.h"
#include "templateparser/templatestextedit.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QHBoxLayout>
#include <QStandardPaths>

TemplateEditorTestWidget::TemplateEditorTestWidget(QWidget *parent)
    : QWidget(parent)
{
    auto lay = new QHBoxLayout(this);
    lay->addWidget(new TemplateParser::TemplatesTextEdit(this));
}

TemplateEditorTestWidget::~TemplateEditorTestWidget() = default;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    auto w = new TemplateEditorTestWidget();
    w->resize(800, 600);

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_templateeditor_gui.cpp"
