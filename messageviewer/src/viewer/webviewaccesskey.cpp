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
#include <KActionCollection>

#include <QLabel>
#include <QHash>
#include <QList>
#include <QWebElement>
#include <QWebView>
#include <QToolTip>
#include <QWebFrame>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QAction>

using namespace MessageViewer;

class MessageViewer::WebViewAccessKeyPrivate
{
public:

    WebViewAccessKeyPrivate(QWebView *webView)
        : mWebView(webView),
          mActionCollection(Q_NULLPTR)
    {

    }
    WebViewAccessKey::AccessKeyState mAccessKeyActivated;
    QList<QLabel *> mAccessKeyLabels;
    QHash<QChar, QWebElement> mAccessKeyNodes;
    QHash<QString, QChar> mDuplicateLinkElements;
    QWebView *mWebView;
    KActionCollection *mActionCollection;
};

static bool isEditableElement(QWebPage *page)
{
    const QWebFrame *frame = (page ? page->currentFrame() : 0);
    QWebElement element = (frame ? frame->findFirstElement(QStringLiteral(":focus")) : QWebElement());
    if (!element.isNull()) {
        const QString tagName(element.tagName());
        if (tagName.compare(QLatin1String("textarea"), Qt::CaseInsensitive) == 0) {
            return true;
        }
        const QString type(element.attribute(QStringLiteral("type")).toLower());
        if (tagName.compare(QLatin1String("input"), Qt::CaseInsensitive) == 0
                && (type.isEmpty() || type == QLatin1String("text") || type == QLatin1String("password"))) {
            return true;
        }
        if (element.evaluateJavaScript(QStringLiteral("this.isContentEditable")).toBool()) {
            return true;
        }
    }
    return false;
}

static bool isHiddenElement(const QWebElement &element)
{
    // width property set to less than zero
    if (element.hasAttribute(QStringLiteral("width")) && element.attribute(QStringLiteral("width")).toInt() < 1) {
        return true;
    }

    // height property set to less than zero
    if (element.hasAttribute(QStringLiteral("height")) && element.attribute(QStringLiteral("height")).toInt() < 1) {
        return true;
    }

    // visibility set to 'hidden' in the element itself or its parent elements.
    if (element.styleProperty(QStringLiteral("visibility"), QWebElement::ComputedStyle).compare(QLatin1String("hidden"), Qt::CaseInsensitive) == 0) {
        return true;
    }

    // display set to 'none' in the element itself or its parent elements.
    if (element.styleProperty(QStringLiteral("display"), QWebElement::ComputedStyle).compare(QLatin1String("none"), Qt::CaseInsensitive) == 0) {
        return true;
    }

    return false;
}


static QString linkElementKey(const QWebElement &element)
{
    if (element.hasAttribute(QStringLiteral("href"))) {
        const QUrl url = element.webFrame()->baseUrl().resolved(element.attribute(QStringLiteral("href")));
        QString linkKey(url.toString());
        if (element.hasAttribute(QStringLiteral("target"))) {
            linkKey += QLatin1Char('+');
            linkKey += element.attribute(QStringLiteral("target"));
        }
        return linkKey;
    }
    return QString();
}

static void handleDuplicateLinkElements(const QWebElement &element, QHash<QString, QChar> *dupLinkList, QChar *accessKey)
{
    if (element.tagName().compare(QLatin1String("A"), Qt::CaseInsensitive) == 0) {
        const QString linkKey(linkElementKey(element));
        // qCDebug(MESSAGEVIEWER_LOG) << "LINK KEY:" << linkKey;
        if (dupLinkList->contains(linkKey)) {
            // qCDebug(MESSAGEVIEWER_LOG) << "***** Found duplicate link element:" << linkKey;
            *accessKey = dupLinkList->value(linkKey);
        } else if (!linkKey.isEmpty()) {
            dupLinkList->insert(linkKey, *accessKey);
        }
        if (linkKey.isEmpty()) {
            *accessKey = QChar();
        }
    }
}


WebViewAccessKey::WebViewAccessKey(QWebView *webView, QObject *parent)
    : QObject(parent),
      d(new MessageViewer::WebViewAccessKeyPrivate(webView))
{

}

WebViewAccessKey::~WebViewAccessKey()
{
    delete d;
}



void WebViewAccessKey::wheelEvent(QWheelEvent *e)
{
    if (d->mAccessKeyActivated == PreActivated && (e->modifiers() & Qt::ControlModifier)) {
        d->mAccessKeyActivated = NotActivated;
    }
}

void WebViewAccessKey::keyPressEvent(QKeyEvent *e)
{
    if (e && d->mWebView->hasFocus()) {
        if (d->mAccessKeyActivated == Activated) {
            if (checkForAccessKey(e)) {
                hideAccessKeys();
                e->accept();
                return;
            }
            hideAccessKeys();
        } else if (e->key() == Qt::Key_Control && e->modifiers() == Qt::ControlModifier && !isEditableElement(d->mWebView->page())) {
            d->mAccessKeyActivated = PreActivated; // Only preactive here, it will be actually activated in key release.
        }
    }
}

void WebViewAccessKey::keyReleaseEvent(QKeyEvent *e)
{
    if (d->mAccessKeyActivated == PreActivated) {
        // Activate only when the CTRL key is pressed and released by itself.
        if (e->key() == Qt::Key_Control && e->modifiers() == Qt::NoModifier) {
            showAccessKeys();
            d->mAccessKeyActivated = Activated;
        } else {
            d->mAccessKeyActivated = NotActivated;
        }
    }
}


void WebViewAccessKey::setActionCollection(KActionCollection *ac)
{
    d->mActionCollection = ac;
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
    QList<QChar> unusedKeys;
    unusedKeys.reserve(10 + ('Z' - 'A' + 1));
    for (char c = 'A'; c <= 'Z'; ++c) {
        unusedKeys << QLatin1Char(c);
    }
    for (char c = '0'; c <= '9'; ++c) {
        unusedKeys << QLatin1Char(c);
    }
    if (d->mActionCollection) {
        Q_FOREACH (QAction *act, d->mActionCollection->actions()) {
            QAction *a = qobject_cast<QAction *>(act);
            if (a) {
                const QKeySequence shortCut = a->shortcut();
                if (!shortCut.isEmpty()) {
                    Q_FOREACH (QChar c, unusedKeys) {
                        if (shortCut.matches(QKeySequence(c)) != QKeySequence::NoMatch) {
                            unusedKeys.removeOne(c);
                        }
                    }
                }
            }
        }
    }
    QList<QWebElement> unLabeledElements;
    QRect viewport = QRect(d->mWebView->page()->mainFrame()->scrollPosition(), d->mWebView->page()->viewportSize());
    const QString selectorQuery(QStringLiteral("a[href],"
                                "area,"
                                "button:not([disabled]),"
                                "input:not([disabled]):not([hidden]),"
                                "label[for],"
                                "legend,"
                                "select:not([disabled]),"
                                "textarea:not([disabled])"));
    QList<QWebElement> result = d->mWebView->page()->mainFrame()->findAllElements(selectorQuery).toList();

    // Priority first goes to elements with accesskey attributes
    Q_FOREACH (const QWebElement &element, result) {
        const QRect geometry = element.geometry();
        if (geometry.size().isEmpty() || !viewport.contains(geometry.topLeft())) {
            continue;
        }
        if (isHiddenElement(element)) {
            continue;    // Do not show access key for hidden elements...
        }
        const QString accessKeyAttribute(element.attribute(QStringLiteral("accesskey")).toUpper());
        if (accessKeyAttribute.isEmpty()) {
            unLabeledElements.append(element);
            continue;
        }
        QChar accessKey;
        for (int i = 0; i < accessKeyAttribute.count(); i += 2) {
            const QChar &possibleAccessKey = accessKeyAttribute[i];
            if (unusedKeys.contains(possibleAccessKey)) {
                accessKey = possibleAccessKey;
                break;
            }
        }
        if (accessKey.isNull()) {
            unLabeledElements.append(element);
            continue;
        }

        handleDuplicateLinkElements(element, &d->mDuplicateLinkElements, &accessKey);
        if (!accessKey.isNull()) {
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
        }
    }

    // Pick an access key first from the letters in the text and then from the
    // list of unused access keys
    Q_FOREACH (const QWebElement &element, unLabeledElements) {
        const QRect geometry = element.geometry();
        if (unusedKeys.isEmpty()
                || geometry.size().isEmpty()
                || !viewport.contains(geometry.topLeft())) {
            continue;
        }
        QChar accessKey;
        const QString text = element.toPlainText().toUpper();
        for (int i = 0; i < text.count(); ++i) {
            const QChar &c = text.at(i);
            if (unusedKeys.contains(c)) {
                accessKey = c;
                break;
            }
        }
        if (accessKey.isNull()) {
            accessKey = unusedKeys.takeFirst();
        }

        handleDuplicateLinkElements(element, &d->mDuplicateLinkElements, &accessKey);
        if (!accessKey.isNull()) {
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
        }
    }

    d->mAccessKeyActivated = (d->mAccessKeyLabels.isEmpty() ? Activated : NotActivated);
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
