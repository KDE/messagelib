/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testwebenginescript.h"
using namespace Qt::Literals::StringLiterals;

#include "webenginescript.h"

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
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

static InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFunction)(Arg))
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

void TestWebEngineScriptPage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                                       const QString &message,
                                                       int lineNumber,
                                                       const QString &sourceID)
{
    Q_UNUSED(level)
    Q_UNUSED(lineNumber)
    Q_UNUSED(sourceID)
    qDebug() << "JAVASCRIPT MESSAGE : " << message;
    // TODO improve it.
    Q_EMIT showConsoleMessage(message);
}

TestWebEngineScript::TestWebEngineScript(QWidget *parent)
    : QWidget(parent)
{
    auto vboxLayout = new QVBoxLayout(this);
    mTestWebEngine = new TestWebEngineScriptView(this);
    auto page = new TestWebEngineScriptPage(this);
    mTestWebEngine->setPage(page);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(u"http://www.kde.org"_s));

    mTestScriptWidget = new TestScriptWidget(this);
    vboxLayout->addWidget(mTestScriptWidget);
    connect(mTestScriptWidget, &TestScriptWidget::executeScript, this, &TestWebEngineScript::slotExecuteScript);
    connect(page, &TestWebEngineScriptPage::showConsoleMessage, this, &TestWebEngineScript::slotShowConsoleMessage);
}

TestWebEngineScript::~TestWebEngineScript() = default;

void TestWebEngineScript::slotShowConsoleMessage(const QString &msg)
{
    Q_UNUSED(msg)
}

void TestWebEngineScript::handleScript(const QVariant &res)
{
    qDebug() << " res" << res;
    // TODO
    // mTestScriptWidget->setResult();
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
    auto testWebEngine = new TestWebEngineScript;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

TestScriptWidget::TestScriptWidget(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);

    auto vScriptLayout = new QVBoxLayout;
    layout->addLayout(vScriptLayout);

    mScriptCombo = new QComboBox;
    vScriptLayout->addWidget(mScriptCombo);
    fillScriptCombo(mScriptCombo);
    connect(mScriptCombo, &QComboBox::currentIndexChanged, this, &TestScriptWidget::slotCurrentIndexChanged);

    mScriptEdit = new QTextEdit;
    mScriptEdit->setAcceptRichText(false);
    vScriptLayout->addWidget(mScriptEdit);

    auto vboxLayout = new QVBoxLayout;
    layout->addLayout(vboxLayout);

    mResultEdit = new QTextEdit;
    mResultEdit->setReadOnly(true);
    vboxLayout->addWidget(mResultEdit);
    auto button = new QPushButton(u"Execute Script"_s, this);
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
    scriptCombo->addItem(u"find all images"_s, WebEngineViewer::WebEngineScript::findAllImages());
    scriptCombo->addItem(u"find all scripts"_s, WebEngineViewer::WebEngineScript::findAllScripts());
    scriptCombo->addItem(u"find all anchors"_s, WebEngineViewer::WebEngineScript::findAllAnchors());
    scriptCombo->addItem(u"find all anchors and forms"_s, WebEngineViewer::WebEngineScript::findAllAnchorsAndForms());
    scriptCombo->addItem(u"search element position"_s, WebEngineViewer::WebEngineScript::searchElementPosition(u"elements"_s));
    scriptCombo->addItem(u"scroll to position"_s, WebEngineViewer::WebEngineScript::scrollToPosition(QPoint()));
    scriptCombo->addItem(u"scroll down"_s, WebEngineViewer::WebEngineScript::scrollDown(0));
    scriptCombo->addItem(u"scroll up"_s, WebEngineViewer::WebEngineScript::scrollUp(0));
    scriptCombo->addItem(u"scroll percentage"_s, WebEngineViewer::WebEngineScript::scrollPercentage(0));
    scriptCombo->addItem(u"Test is bottom"_s, WebEngineViewer::WebEngineScript::isScrolledToBottom());
#if 0
    WebEngineViewer::WebEngineScript::setElementByIdVisible(const QString &elementStr, bool visibility);
    WebEngineViewer::WebEngineScript::setStyleToElement(const QString &elementStr, const QString &style);
    WebEngineViewer::WebEngineScript::scrollToRelativePosition(int pos);
    WebEngineViewer::WebEngineScript::removeStyleToElement(const QString &element);
    WebEngineViewer::WebEngineScript::replaceInnerHtml(const QString &field, const QString &html, bool doShow);
#endif
    // TODO
}

void TestScriptWidget::setResult(const QString &res)
{
    mResultEdit->setText(res);
}

QString TestScriptWidget::script() const
{
    return mScriptEdit->toPlainText();
}

#include "moc_testwebenginescript.cpp"
