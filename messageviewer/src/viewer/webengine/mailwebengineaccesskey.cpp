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

#include "mailwebengineaccesskey.h"

#include <KActionCollection>
#include <QKeyEvent>
#include <QLabel>

using namespace MessageViewer;

class MessageViewer::MailWebEngineAccessKeyPrivate
{
public:
    enum AccessKeyState {
        NotActivated,
        PreActivated,
        Activated
    };

    MailWebEngineAccessKeyPrivate()
        : mAccessKeyActivated(NotActivated),
          mActionCollection(Q_NULLPTR)
    {

    }
    QList<QLabel *> mAccessKeyLabels;
    //QHash<QChar, QWebElement> mAccessKeyNodes;
    QHash<QString, QChar> mDuplicateLinkElements;
    //QWebView *mWebView;
    AccessKeyState mAccessKeyActivated;
    KActionCollection *mActionCollection;
};

MailWebEngineAccessKey::MailWebEngineAccessKey(QObject *parent)
    : QObject(parent),
      d(new MessageViewer::MailWebEngineAccessKeyPrivate)
{

}

MailWebEngineAccessKey::~MailWebEngineAccessKey()
{
    delete d;
}

void MailWebEngineAccessKey::setActionCollection(KActionCollection *ac)
{
    d->mActionCollection = ac;
}

void MailWebEngineAccessKey::wheelEvent(QWheelEvent *e)
{
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::PreActivated && (e->modifiers() & Qt::ControlModifier)) {
        d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::NotActivated;
    }
}

void MailWebEngineAccessKey::resizeEvent(QResizeEvent *)
{
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::Activated) {
        //FIXME hideAccessKeys();
    }
}

void MailWebEngineAccessKey::keyPressEvent(QKeyEvent *e)
{
#if 0
    if (e && d->mWebView->hasFocus()) {
        if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::Activated) {
#if 0 //FIXME
            if (checkForAccessKey(e)) {
                hideAccessKeys();
                e->accept();
                return;
            }
            hideAccessKeys();
#endif
        } else if (e->key() == Qt::Key_Control && e->modifiers() == Qt::ControlModifier && !isEditableElement(d->mWebView->page())) {
            d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::PreActivated; // Only preactive here, it will be actually activated in key release.
        }
    }
#endif
}

void MailWebEngineAccessKey::keyReleaseEvent(QKeyEvent *e)
{
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::PreActivated) {
        // Activate only when the CTRL key is pressed and released by itself.
        if (e->key() == Qt::Key_Control && e->modifiers() == Qt::NoModifier) {
            //FIXME showAccessKeys();
            d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::Activated;
        } else {
            d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::NotActivated;
        }
    }
}

void MailWebEngineAccessKey::hideAccessKeys()
{
    if (!d->mAccessKeyLabels.isEmpty()) {
        for (int i = 0, count = d->mAccessKeyLabels.count(); i < count; ++i) {
            QLabel *label = d->mAccessKeyLabels[i];
            label->hide();
            label->deleteLater();
        }
        d->mAccessKeyLabels.clear();
        //FIXME d->mAccessKeyNodes.clear();
        d->mDuplicateLinkElements.clear();
        d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::NotActivated;
        //FIXME d->mWebView->update();
    }
}

 #if 0
void MailWebEngineAccessKey::makeAccessKeyLabel(QChar accessKey, const QWebElement &element)
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
#endif

bool MailWebEngineAccessKey::checkForAccessKey(QKeyEvent *event)
{
    if (d->mAccessKeyLabels.isEmpty()) {
        return false;
    }
    QString text = event->text();
    if (text.isEmpty()) {
        return false;
    }
#if 0
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
#else
    return false;
#endif
}

void MailWebEngineAccessKey::showAccessKeys()
{
#if 0
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

    d->mAccessKeyActivated = (d->mAccessKeyLabels.isEmpty() ? WebViewAccessKeyPrivate::Activated : WebViewAccessKeyPrivate::NotActivated);
#endif
}
