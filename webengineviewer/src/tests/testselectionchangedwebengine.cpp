/*
   Copyright (C) 2017-2018 Laurent Montel <montel@kde.org>

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

#include "testselectionchangedwebengine.h"

#include <QApplication>
#include <QVBoxLayout>
#include <webengineview.h>
#include <QTextEdit>
#include <QDebug>

TestSelectionChangedEngine::TestSelectionChangedEngine(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *layout = new QHBoxLayout;
    vbox->addLayout(layout);
    pageView = new WebEngineViewer::WebEngineView(this);
    pageView->load(QUrl(QStringLiteral("http://www.planetkde.org")));
    connect(pageView, &WebEngineViewer::WebEngineView::selectionChanged, this, &TestSelectionChangedEngine::slotSelectionChanged);
    layout->addWidget(pageView);

    QTextEdit *edit = new QTextEdit(this);
    vbox->addWidget(edit);
}

TestSelectionChangedEngine::~TestSelectionChangedEngine()
{
}

void TestSelectionChangedEngine::slotSelectionChanged()
{
    qDebug() << " void TestSelectionChangedEngine::slotSelectionChanged()";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestSelectionChangedEngine *testWebEngine = new TestSelectionChangedEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
