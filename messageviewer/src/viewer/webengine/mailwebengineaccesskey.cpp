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
#include "mailwebengineaccesskeyanchor.h"
#include "mailwebengineaccesskeyutils.h"


#include <KActionCollection>
#include <QKeyEvent>
#include <QLabel>
#include <QAction>
#include <QWebEngineView>
#include <QToolTip>
#include <QCoreApplication>
#include <QDebug>
using namespace MessageViewer;
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

class MessageViewer::MailWebEngineAccessKeyPrivate
{
public:
    enum AccessKeyState {
        NotActivated,
        PreActivated,
        Activated
    };

    MailWebEngineAccessKeyPrivate(QWebEngineView *webEngine)
        : mWebEngine(webEngine),
          mAccessKeyActivated(NotActivated),
          mActionCollection(Q_NULLPTR)
    {

    }
    QList<QLabel *> mAccessKeyLabels;
    QHash<QChar, MessageViewer::MailWebEngineAccessKeyAnchor> mAccessKeyNodes;
    QHash<QString, QChar> mDuplicateLinkElements;
    QWebEngineView *mWebEngine;
    AccessKeyState mAccessKeyActivated;
    KActionCollection *mActionCollection;
};

#if 0
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


#endif
static QString linkElementKey(const MessageViewer::MailWebEngineAccessKeyAnchor &element, const QUrl &baseUrl)
{
    //qDebug()<<" element.href()"<<element.href();
    if (!element.href().isEmpty()) {
        const QUrl url = baseUrl.resolved(element.href());
        //qDebug()<< "URL " << url;
        QString linkKey(url.toString());
        if (!element.target().isEmpty()) {
            linkKey += QLatin1Char('+');
            linkKey += element.target();
        }
        return linkKey;
    }
    return QString();
}

static void handleDuplicateLinkElements(const MessageViewer::MailWebEngineAccessKeyAnchor &element, QHash<QString, QChar> *dupLinkList, QChar *accessKey, const QUrl &baseUrl)
{
    if (element.tagName().compare(QLatin1String("A"), Qt::CaseInsensitive) == 0) {
        const QString linkKey(linkElementKey(element, baseUrl));
        //qDebug() << "LINK KEY:" << linkKey;
        if (dupLinkList->contains(linkKey)) {
            //qDebug() << "***** Found duplicate link element:" << linkKey;
            *accessKey = dupLinkList->value(linkKey);
        } else if (!linkKey.isEmpty()) {
            dupLinkList->insert(linkKey, *accessKey);
        }
        if (linkKey.isEmpty()) {
            *accessKey = QChar();
        }
    }
}

static bool isHiddenElement(const MessageViewer::MailWebEngineAccessKeyAnchor &element)
{
    // width property set to less than zero
    if (element.boundingRect().width() < 1) {
        return true;
    }
    // height property set to less than zero
    if (element.boundingRect().height()  < 1) {
        return true;
    }
#if 0

    // visibility set to 'hidden' in the element itself or its parent elements.
    if (element.styleProperty(QStringLiteral("visibility"), QWebElement::ComputedStyle).compare(QLatin1String("hidden"), Qt::CaseInsensitive) == 0) {
        return true;
    }

    // display set to 'none' in the element itself or its parent elements.
    if (element.styleProperty(QStringLiteral("display"), QWebElement::ComputedStyle).compare(QLatin1String("none"), Qt::CaseInsensitive) == 0) {
        return true;
    }
#endif
    return false;
}

MailWebEngineAccessKey::MailWebEngineAccessKey(QWebEngineView *webEngine, QObject *parent)
    : QObject(parent),
      d(new MessageViewer::MailWebEngineAccessKeyPrivate(webEngine))
{
    qDebug()<<" MailWebEngineAccessKey::MailWebEngineAccessKey(QWebEngineView *webEngine, QObject *parent)";
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
    hideAccessKeys();
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::PreActivated && (e->modifiers() & Qt::ControlModifier)) {
        d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::NotActivated;
    }
}

void MailWebEngineAccessKey::resizeEvent(QResizeEvent *)
{
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::Activated) {
        hideAccessKeys();
    }
}

void MailWebEngineAccessKey::keyPressEvent(QKeyEvent *e)
{
    if (e && d->mWebEngine->hasFocus()) {
        if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::Activated) {
            if (checkForAccessKey(e)) {
                hideAccessKeys();
                e->accept();
                return;
            }
            hideAccessKeys();
        } else if (e->key() == Qt::Key_Control && e->modifiers() == Qt::ControlModifier
           #if 0 //FIXME
                   && !isEditableElement(d->mWebView->page())
           #endif
                   ) {
            d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::PreActivated; // Only preactive here, it will be actually activated in key release.
        }
    }
}

void MailWebEngineAccessKey::keyReleaseEvent(QKeyEvent *e)
{
    qDebug()<<" void MailWebEngineAccessKey::keyReleaseEvent(QKeyEvent *e)";
    if (d->mAccessKeyActivated == MailWebEngineAccessKeyPrivate::PreActivated) {
        // Activate only when the CTRL key is pressed and released by itself.
        if (e->key() == Qt::Key_Control && e->modifiers() == Qt::NoModifier) {
            showAccessKeys();

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
        d->mAccessKeyNodes.clear();
        d->mDuplicateLinkElements.clear();
        d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::NotActivated;
        d->mWebEngine->update();
    }
}

void MailWebEngineAccessKey::makeAccessKeyLabel(QChar accessKey, const MessageViewer::MailWebEngineAccessKeyAnchor &element)
{
    //qDebug()<<" void MailWebEngineAccessKey::makeAccessKeyLabel(QChar accessKey, const MessageViewer::MailWebEngineAccessKeyAnchor &element)";
    QLabel *label = new QLabel(d->mWebEngine);
    QFont font(label->font());
    font.setBold(true);
    label->setFont(font);
    label->setText(accessKey);
    QFontMetrics metric(label->font());
    label->setFixedWidth(metric.width(QStringLiteral("WW")));
    label->setPalette(QToolTip::palette());
    label->setAutoFillBackground(true);
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPoint point = element.boundingRect().center();
    qDebug()<<" point label"<<point;
#if 0
    point -= d->mWebView->page()->mainFrame()->scrollPosition();
#endif
    label->move(point);
    label->show();
    point.setX(point.x() - label->width() / 2);
    qDebug() << "new point "<<point;
    label->move(point);
    d->mAccessKeyLabels.append(label);
    d->mAccessKeyNodes.insertMulti(accessKey, element);
}

bool MailWebEngineAccessKey::checkForAccessKey(QKeyEvent *event)
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
        MessageViewer::MailWebEngineAccessKeyAnchor element = d->mAccessKeyNodes[key];
        if (element.tagName().compare(QLatin1String("A"), Qt::CaseInsensitive) == 0) {
            const QString linkKey(linkElementKey(element, d->mWebEngine->url()));
            if (!linkKey.isEmpty()) {
                //qDebug()<<" MailWebEngineAccessKey::checkForAccessKey****"<<linkKey;
                Q_EMIT openUrl(QUrl(linkKey));
                handled = true;
            }
        }
    }
    return handled;
}

void MailWebEngineAccessKey::handleSearchAccessKey(const QVariant &res)
{
    //qDebug() << " void MailWebEngineAccessKey::handleSearchAccessKey(const QVariant &res)" << res;
    const QVariantList lst = res.toList();
    QVector<MessageViewer::MailWebEngineAccessKeyAnchor> anchorList;
    anchorList.reserve(lst.count());
    Q_FOREACH(const QVariant &var, lst) {
        //qDebug()<<" var"<<var;
        anchorList << MessageViewer::MailWebEngineAccessKeyAnchor(var);
    }


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
    QVector<MessageViewer::MailWebEngineAccessKeyAnchor> unLabeledElements;
    QRect viewport = d->mWebEngine->rect();//QRect(d->mWebEngine->page()->mainFrame()->scrollPosition(), d->mWebEngine->page()->viewportSize());
    Q_FOREACH (const MessageViewer::MailWebEngineAccessKeyAnchor &element, anchorList) {
        const QRect geometry = element.boundingRect();
        if (geometry.size().isEmpty() || !viewport.contains(geometry.topLeft())) {
            continue;
        }
        if (isHiddenElement(element)) {
            continue;    // Do not show access key for hidden elements...
        }
        const QString accessKeyAttribute(element.accessKey().toUpper());
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

        handleDuplicateLinkElements(element, &d->mDuplicateLinkElements, &accessKey, d->mWebEngine->url());
        if (!accessKey.isNull()) {
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
        }
    }

    // Pick an access key first from the letters in the text and then from the
    // list of unused access keys
    Q_FOREACH (const MessageViewer::MailWebEngineAccessKeyAnchor &element, unLabeledElements) {
        const QRect geometry = element.boundingRect();
        if (unusedKeys.isEmpty()
                || geometry.size().isEmpty()
                || !viewport.contains(geometry.topLeft())) {
            continue;
        }
        QChar accessKey;
        const QString text = element.innerText().toUpper();
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

        handleDuplicateLinkElements(element, &d->mDuplicateLinkElements, &accessKey, d->mWebEngine->url());
        if (!accessKey.isNull()) {
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
        }
    }
    d->mAccessKeyActivated = (!d->mAccessKeyLabels.isEmpty() ? MailWebEngineAccessKeyPrivate::Activated : MailWebEngineAccessKeyPrivate::NotActivated);
}

void MailWebEngineAccessKey::showAccessKeys()
{
    d->mAccessKeyActivated = MailWebEngineAccessKeyPrivate::Activated;
    d->mWebEngine->page()->runJavaScript(MessageViewer::MailWebEngineAccessKeyUtils::script(), invoke(this, &MailWebEngineAccessKey::handleSearchAccessKey));
}

