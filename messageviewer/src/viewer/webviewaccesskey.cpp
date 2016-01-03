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

#include "webviewaccesskey.h"
#include <QLabel>
#include <QHash>
#include <QList>
#include <QWebElement>
#include <QWebView>
#include <QToolTip>
#include <QWebFrame>
#include <QMouseEvent>
#include <QCoreApplication>

using namespace MessageViewer;

class MessageViewer::WebViewAccessKeyPrivate
{
public:

    WebViewAccessKeyPrivate(QWebView *webView)
        : mWebView(webView)
    {

    }
    WebViewAccessKey::AccessKeyState mAccessKeyActivated;
    QList<QLabel *> mAccessKeyLabels;
    QHash<QChar, QWebElement> mAccessKeyNodes;
    QHash<QString, QChar> mDuplicateLinkElements;
    QWebView *mWebView;
};

WebViewAccessKey::WebViewAccessKey(QWebView *webView, QObject *parent)
    : QObject(parent),
      d(new MessageViewer::WebViewAccessKeyPrivate(webView))
{

}

WebViewAccessKey::~WebViewAccessKey()
{
    delete d;
}

bool MessageViewer::WebViewAccessKey::checkForAccessKey(QKeyEvent *event)
{
    if (d->mAccessKeyLabels.isEmpty()) {
        return false;
    }
    QString text = event->text();
    if (text.isEmpty()) {
        return false;
    }
    QChar key = text.at(0).toUpper();
    bool handled = false;
    if (d->mAccessKeyNodes.contains(key)) {
        QWebElement element = d->mAccessKeyNodes[key];
        QPoint p = element.geometry().center();
        QWebFrame *frame = element.webFrame();
        Q_ASSERT(frame);
        do {
            p -= frame->scrollPosition();
            frame = frame->parentFrame();
        } while (frame && frame != d->mWebView->page()->mainFrame());
        QMouseEvent pevent(QEvent::MouseButtonPress, p, Qt::LeftButton, 0, 0);
        QCoreApplication::sendEvent(this, &pevent);
        QMouseEvent revent(QEvent::MouseButtonRelease, p, Qt::LeftButton, 0, 0);
        QCoreApplication::sendEvent(this, &revent);
        handled = true;
    }
    return handled;
}

void WebViewAccessKey::showAccessKeys()
{
    //TODO
}

void WebViewAccessKey::makeAccessKeyLabel(QChar accessKey, const QWebElement &element)
{
    QLabel *label = new QLabel(d->mWebView);
    QFont font(label->font());
    font.setBold(true);
    label->setFont(font);
    label->setText(accessKey);
    label->setPalette(QToolTip::palette());
    label->setAutoFillBackground(true);
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPoint point = element.geometry().center();
    point -= d->mWebView->page()->mainFrame()->scrollPosition();
    label->move(point);
    label->show();
    point.setX(point.x() - label->width() / 2);
    label->move(point);
    d->mAccessKeyLabels.append(label);
    d->mAccessKeyNodes.insertMulti(accessKey, element);

}

void WebViewAccessKey::hideAccessKeys()
{
    if (!d->mAccessKeyLabels.isEmpty()) {
        for (int i = 0, count = d->mAccessKeyLabels.count(); i < count; ++i) {
            QLabel *label = d->mAccessKeyLabels[i];
            label->hide();
            label->deleteLater();
        }
        d->mAccessKeyLabels.clear();
        d->mAccessKeyNodes.clear();
        d->mDuplicateLinkElements.clear();
        d->mAccessKeyActivated = WebViewAccessKey::NotActivated;
        d->mWebView->update();
    }
}
