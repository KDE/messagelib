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
#include "webengine/webenginescript.h"

#include <QApplication>
#include <QPushButton>
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
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
}

TestWebEngineScriptPage::TestWebEngineScriptPage(QObject *parent)
    : QWebEnginePage(parent)
{
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
}

void TestWebEngineScriptPage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    qDebug() << "JAVASCRIPT MESSAGE : "<< message;
}

TestWebEngineScript::TestWebEngineScript(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    mTestWebEngine = new TestWebEngineScriptView(this);
    mTestWebEngine->setPage(new TestWebEngineScriptPage(this));

    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));

    mTestScriptWidget = new TestScriptWidget(this);
    vboxLayout->addWidget(mTestScriptWidget);
    connect(mTestScriptWidget, &TestScriptWidget::executeScript, this, &TestWebEngineScript::slotExecuteScript);
}

TestWebEngineScript::~TestWebEngineScript()
{

}

void TestWebEngineScript::handleScript(const QVariant &res)
{
    qDebug() << " res" << res;
    //TODO
    //mTestScriptWidget->setResult();
}

void TestWebEngineScript::slotExecuteScript()
{
    const QString script = mTestScriptWidget->script();
    if (!script.isEmpty()) {
        qDebug() << " script" << script;
        mTestWebEngine->page()->runJavaScript(script, invoke(this, &TestWebEngineScript::handleScript));
    }
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
    mScriptEdit->setAcceptRichText(false);
    layout->addWidget(mScriptEdit);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    layout->addLayout(vboxLayout);

    mResultEdit = new QTextEdit;
    mResultEdit->setReadOnly(true);
    vboxLayout->addWidget(mResultEdit);
    QPushButton *button = new QPushButton(QStringLiteral("Execute Script"), this);
    connect(button, &QPushButton::clicked, this, &TestScriptWidget::executeScript);
    vboxLayout->addWidget(button);
}

void TestScriptWidget::setResult(const QString &res)
{
    mResultEdit->setText(res);
}

QString TestScriptWidget::script() const
{
    return mScriptEdit->toPlainText();
}
