/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "testjquerysupportwebengine.h"
#include <QApplication>
#include <WebEngineViewer/WebEnginePage>
#include <webengineview.h>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

TestJQuerySupportWebEngine::TestJQuerySupportWebEngine(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    pageView = new WebEngineViewer::WebEngineView(this);
    vboxLayout->addWidget(pageView);

    pageView->initializeJQueryScript();
    mEditor = new QTextEdit(this);
    mEditor->setAcceptRichText(false);
    mEditor->setPlainText(QStringLiteral("qt.jQuery('img').each( function () { qt.jQuery(this).css('-webkit-transition', '-webkit-transform 2s'); qt.jQuery(this).css('-webkit-transform', 'rotate(180deg)') } ); undefined"));
    vboxLayout->addWidget(mEditor);

    QPushButton *executeQuery = new QPushButton(QStringLiteral("Execute Query"), this);
    connect(executeQuery, &QPushButton::clicked, this, &TestJQuerySupportWebEngine::slotExecuteQuery);
    vboxLayout->addWidget(executeQuery);

    pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
}

TestJQuerySupportWebEngine::~TestJQuerySupportWebEngine()
{

}

void TestJQuerySupportWebEngine::slotExecuteQuery()
{
    const QString code = mEditor->toPlainText();
    if (!code.isEmpty()) {
        pageView->page()->runJavaScript(code);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestJQuerySupportWebEngine *testWebEngine = new TestJQuerySupportWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
