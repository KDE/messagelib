/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#include "testwebenginescript.h"
#include "webenginescript.h"

#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QWebEngineSettings>
#include <QComboBox>

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
    Q_UNUSED(level);
    Q_UNUSED(lineNumber);
    Q_UNUSED(sourceID);
    qDebug() << "JAVASCRIPT MESSAGE : " << message;
    //TODO improve it.
    Q_EMIT showConsoleMessage(message);
}

TestWebEngineScript::TestWebEngineScript(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    mTestWebEngine = new TestWebEngineScriptView(this);
    TestWebEngineScriptPage *page = new TestWebEngineScriptPage(this);
    mTestWebEngine->setPage(page);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));

    mTestScriptWidget = new TestScriptWidget(this);
    vboxLayout->addWidget(mTestScriptWidget);
    connect(mTestScriptWidget, &TestScriptWidget::executeScript, this, &TestWebEngineScript::slotExecuteScript);
    connect(page, &TestWebEngineScriptPage::showConsoleMessage, this, &TestWebEngineScript::slotShowConsoleMessage);
}

TestWebEngineScript::~TestWebEngineScript()
{
}

void TestWebEngineScript::slotShowConsoleMessage(const QString &msg)
{
    Q_UNUSED(msg);
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

    QVBoxLayout *vScriptLayout = new QVBoxLayout;
    layout->addLayout(vScriptLayout);

    mScriptCombo = new QComboBox;
    vScriptLayout->addWidget(mScriptCombo);
    fillScriptCombo(mScriptCombo);
    connect(mScriptCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged(int)));

    mScriptEdit = new QTextEdit;
    mScriptEdit->setAcceptRichText(false);
    vScriptLayout->addWidget(mScriptEdit);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    layout->addLayout(vboxLayout);

    mResultEdit = new QTextEdit;
    mResultEdit->setReadOnly(true);
    vboxLayout->addWidget(mResultEdit);
    QPushButton *button = new QPushButton(QStringLiteral("Execute Script"), this);
    connect(button, &QPushButton::clicked, this, &TestScriptWidget::executeScript);
    vboxLayout->addWidget(button);
}

void TestScriptWidget::slotCurrentIndexChanged(int index)
{
    if (index != -1) {
        const QString str = mScriptCombo->currentData().toString();
        mScriptEdit->setText(str);
    }
}

void TestScriptWidget::fillScriptCombo(QComboBox *scriptCombo)
{
    scriptCombo->addItem(QStringLiteral("find all images"), WebEngineViewer::WebEngineScript::findAllImages());
    scriptCombo->addItem(QStringLiteral("find all scripts"), WebEngineViewer::WebEngineScript::findAllScripts());
    scriptCombo->addItem(QStringLiteral("find all anchors"), WebEngineViewer::WebEngineScript::findAllAnchors());
    scriptCombo->addItem(QStringLiteral("find all anchors and forms"), WebEngineViewer::WebEngineScript::findAllAnchorsAndForms());
    scriptCombo->addItem(QStringLiteral("search element position"), WebEngineViewer::WebEngineScript::searchElementPosition(QStringLiteral("elements")));
    scriptCombo->addItem(QStringLiteral("scroll to position"), WebEngineViewer::WebEngineScript::scrollToPosition(QPoint()));
    scriptCombo->addItem(QStringLiteral("scroll down"), WebEngineViewer::WebEngineScript::scrollDown(0));
    scriptCombo->addItem(QStringLiteral("scroll up"), WebEngineViewer::WebEngineScript::scrollUp(0));
    scriptCombo->addItem(QStringLiteral("scroll percentage"), WebEngineViewer::WebEngineScript::scrollPercentage(0));
    scriptCombo->addItem(QStringLiteral("Test is bottom"), WebEngineViewer::WebEngineScript::isScrolledToBottom());
#if 0
    WebEngineViewer::WebEngineScript::setElementByIdVisible(const QString & elementStr, bool visibility);
    WebEngineViewer::WebEngineScript::setStyleToElement(const QString & elementStr, const QString & style);
    WebEngineViewer::WebEngineScript::scrollToRelativePosition(int pos);
    WebEngineViewer::WebEngineScript::removeStyleToElement(const QString & element);
    WebEngineViewer::WebEngineScript::injectAttachments(const QString & delayedHtml, const QString & element);
    WebEngineViewer::WebEngineScript::toggleFullAddressList(const QString & field, const QString & html, bool doShow);
    WebEngineViewer::WebEngineScript::replaceInnerHtml(const QString & field, const QString & html, bool doShow);
    WebEngineViewer::WebEngineScript::updateToggleFullAddressList(const QString & field, bool doShow);
#endif
    //TODO
}

void TestScriptWidget::setResult(const QString &res)
{
    mResultEdit->setText(res);
}

QString TestScriptWidget::script() const
{
    return mScriptEdit->toPlainText();
}
