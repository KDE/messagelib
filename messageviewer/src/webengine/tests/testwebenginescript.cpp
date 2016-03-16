/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "testwebenginescript.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QWebEngineSettings>

template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFunction)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFunction)(result);
    }
};

template<typename Arg, typename R, typename C>

InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFunction)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFunction};
    return wrapper;
}


TestWebEngineScriptView::TestWebEngineScriptView(QWidget *parent)
    : QWebEngineView(parent)
{
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
}

TestWebEngineScript::TestWebEngineScript(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    mTestWebEngine = new TestWebEngineScriptView(this);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));

    mTestScriptWidget = new TestScriptWidget(this);
    vboxLayout->addWidget(mTestScriptWidget);
}

TestWebEngineScript::~TestWebEngineScript()
{

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestWebEngineScript *testWebEngine = new TestWebEngineScript;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

TestScriptWidget::TestScriptWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    mScriptEdit = new QTextEdit;
    layout->addWidget(mScriptEdit);
    mResultEdit = new QTextEdit;
    mResultEdit->setReadOnly(true);
    layout->addWidget(mResultEdit);
}
