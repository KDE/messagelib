/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testselectionchangedwebengine.h"

#include "webengineview.h"
#include <QApplication>
#include <QDebug>
#include <QTextEdit>
#include <QVBoxLayout>

TestSelectionChangedEngine::TestSelectionChangedEngine(QWidget *parent)
    : QWidget(parent)
{
    auto vbox = new QVBoxLayout(this);
    auto layout = new QHBoxLayout;
    vbox->addLayout(layout);
    pageView = new WebEngineViewer::WebEngineView(this);
    pageView->load(QUrl(QStringLiteral("http://www.planetkde.org")));
    connect(pageView, &WebEngineViewer::WebEngineView::selectionChanged, this, &TestSelectionChangedEngine::slotSelectionChanged);
    layout->addWidget(pageView);

    auto edit = new QTextEdit(this);
    vbox->addWidget(edit);
}

TestSelectionChangedEngine::~TestSelectionChangedEngine() = default;

void TestSelectionChangedEngine::slotSelectionChanged()
{
    qDebug() << " void TestSelectionChangedEngine::slotSelectionChanged()";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto testWebEngine = new TestSelectionChangedEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

#include "moc_testselectionchangedwebengine.cpp"
