/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "templateparseremailrequester.h"
#include "templateparser/templatestextedit.h"
#include "templateparseremailaddressrequesterinterfacewidget.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QHBoxLayout>
#include <QStandardPaths>

TemplateParserEmailRequesterTestWidget::TemplateParserEmailRequesterTestWidget(QWidget *parent)
    : QWidget(parent)
{
    auto lay = new QHBoxLayout(this);
    auto w = new TemplateParser::TemplateParserEmailAddressRequesterInterfaceWidget(this);
    lay->addWidget(w);
}

TemplateParserEmailRequesterTestWidget::~TemplateParserEmailRequesterTestWidget() = default;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    auto w = new TemplateParserEmailRequesterTestWidget();
    w->resize(800, 600);

    w->show();
    app.exec();
    delete w;
    return 0;
}

#include "moc_templateparseremailrequester.cpp"
