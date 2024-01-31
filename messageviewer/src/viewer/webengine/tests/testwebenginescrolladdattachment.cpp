/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testwebenginescrolladdattachment.h"

#include <KActionCollection>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineSettings>

#include "messageviewer/mailwebengineview.h"

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

TestWebEngineScrollAddAttachment::TestWebEngineScrollAddAttachment(QWidget *parent)
    : QWidget(parent)
{
    auto vboxLayout = new QVBoxLayout(this);

    mTestWebEngine = new MessageViewer::MailWebEngineView(new KActionCollection(this), this);
    mTestWebEngine->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));
    auto scrollToButton = new QPushButton(QStringLiteral("Scroll to Attachment"), this);
    vboxLayout->addWidget(scrollToButton);
    connect(scrollToButton, &QPushButton::clicked, this, &TestWebEngineScrollAddAttachment::slotScrollToAttachment);
}

void TestWebEngineScrollAddAttachment::slotScrollToAttachment()
{
    mTestWebEngine->scrollToAnchor(QStringLiteral("module"));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto testWebEngine = new TestWebEngineScrollAddAttachment;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

#include "moc_testwebenginescrolladdattachment.cpp"
