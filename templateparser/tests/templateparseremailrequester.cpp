/*
  SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "templateparseremailrequester.h"
#include "templateparser/templatestextedit.h"
#include "templateparseremailaddressrequesterinterfacewidget.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

TemplateParserEmailRequesterTestWidget::TemplateParserEmailRequesterTestWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    TemplateParser::TemplateParserEmailAddressRequesterInterfaceWidget *w = new TemplateParser::TemplateParserEmailAddressRequesterInterfaceWidget(this);
    lay->addWidget(w);
}

TemplateParserEmailRequesterTestWidget::~TemplateParserEmailRequesterTestWidget()
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

    TemplateParserEmailRequesterTestWidget *w = new TemplateParserEmailRequesterTestWidget();
    w->resize(800, 600);

    w->show();
    app.exec();
    delete w;
    return 0;
}
