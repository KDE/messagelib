/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineaccesskey.h"
#include "webengineaccesskeyanchor.h"
#include "webengineaccesskeyutils.h"
#include "webenginemanagescript.h"

#include <KActionCollection>
#include <QAction>
#include <QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QToolTip>
#include <QVector>
#include <QWebEngineView>
using namespace WebEngineViewer;
template<typename Arg, typename R, typename C>
struct InvokeWrapperWebAccessKey {
    R *receiver;
    void (C::*memberFunction)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFunction)(result);
    }
};

template<typename Arg, typename R, typename C>

InvokeWrapperWebAccessKey<Arg, R, C> invokeWebAccessKey(R *receiver, void (C::*memberFunction)(Arg))
{
    InvokeWrapperWebAccessKey<Arg, R, C> wrapper = {receiver, memberFunction};
    return wrapper;
}

class WebEngineViewer::WebEngineAccessKeyPrivate
{
public:
    enum AccessKeyState { NotActivated, PreActivated, Activated };

    WebEngineAccessKeyPrivate(WebEngineAccessKey *qq, QWebEngineView *webEngine)
        : mWebEngine(webEngine)
        , q(qq)
    {
    }

    void makeAccessKeyLabel(QChar accessKey, const WebEngineViewer::WebEngineAccessKeyAnchor &element);
    bool checkForAccessKey(QKeyEvent *event);
    QVector<QLabel *> mAccessKeyLabels;
    QMultiHash<QChar, WebEngineViewer::WebEngineAccessKeyAnchor> mAccessKeyNodes;
    QHash<QString, QChar> mDuplicateLinkElements;
    QWebEngineView *const mWebEngine;
    AccessKeyState mAccessKeyActivated = NotActivated;
    KActionCollection *mActionCollection = nullptr;
    WebEngineAccessKey *const q = nullptr;
};

static QString linkElementKey(const WebEngineViewer::WebEngineAccessKeyAnchor &element, const QUrl &baseUrl)
{
    // qDebug()<<" element.href()"<<element.href();
    if (!element.href().isEmpty()) {
        const QUrl url = baseUrl.resolved(QUrl(element.href()));
        // qDebug()<< "URL " << url;
        QString linkKey(url.toString());
        if (!element.target().isEmpty()) {
            linkKey += QLatin1Char('+');
            linkKey += element.target();
        }
        return linkKey;
    }
    return QString();
}

static void
handleDuplicateLinkElements(const WebEngineViewer::WebEngineAccessKeyAnchor &element, QHash<QString, QChar> *dupLinkList, QChar *accessKey, const QUrl &baseUrl)
{
    if (element.tagName().compare(QLatin1String("A"), Qt::CaseInsensitive) == 0) {
        const QString linkKey(linkElementKey(element, baseUrl));
        // qDebug() << "LINK KEY:" << linkKey;
        if (dupLinkList->contains(linkKey)) {
            // qDebug() << "***** Found duplicate link element:" << linkKey;
            *accessKey = dupLinkList->value(linkKey);
        } else if (!linkKey.isEmpty()) {
            dupLinkList->insert(linkKey, *accessKey);
        }
        if (linkKey.isEmpty()) {
            *accessKey = QChar();
        }
    }
}

static bool isHiddenElement(const WebEngineViewer::WebEngineAccessKeyAnchor &element)
{
    // width or height property set to less than zero
    if (element.boundingRect().width() < 1 || element.boundingRect().height() < 1) {
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

bool WebEngineAccessKeyPrivate::checkForAccessKey(QKeyEvent *event)
{
    if (mAccessKeyLabels.isEmpty()) {
        return false;
    }
    QString text = event->text();
    if (text.isEmpty()) {
        return false;
    }
    QChar key = text.at(0).toUpper();
    bool handled = false;
    if (mAccessKeyNodes.contains(key)) {
        WebEngineViewer::WebEngineAccessKeyAnchor element = mAccessKeyNodes.value(key);
        if (element.tagName().compare(QLatin1String("A"), Qt::CaseInsensitive) == 0) {
            const QString linkKey(linkElementKey(element, mWebEngine->url()));
            if (!linkKey.isEmpty()) {
                // qDebug()<<" WebEngineAccessKey::checkForAccessKey****"<<linkKey;
                Q_EMIT q->openUrl(QUrl(linkKey));
                handled = true;
            }
        }
    }
    return handled;
}

void WebEngineAccessKeyPrivate::makeAccessKeyLabel(QChar accessKey, const WebEngineViewer::WebEngineAccessKeyAnchor &element)
{
    // qDebug()<<" void WebEngineAccessKey::makeAccessKeyLabel(QChar accessKey, const WebEngineViewer::MailWebEngineAccessKeyAnchor &element)";
    auto label = new QLabel(mWebEngine);
    QFont font(label->font());
    font.setBold(true);
    label->setFont(font);
    label->setText(accessKey);
    QFontMetrics metric(label->font());
    label->setFixedWidth(metric.boundingRect(QStringLiteral("WW")).width());
    label->setPalette(QToolTip::palette());
    label->setAutoFillBackground(true);
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPoint point = element.boundingRect().center();
    label->move(point);
    label->show();
    point.setX(point.x() - label->width() / 2);
    label->move(point);
    mAccessKeyLabels.append(label);
    mAccessKeyNodes.insert(accessKey, element);
}

WebEngineAccessKey::WebEngineAccessKey(QWebEngineView *webEngine, QObject *parent)
    : QObject(parent)
    , d(new WebEngineViewer::WebEngineAccessKeyPrivate(this, webEngine))
{
    // qDebug() << " WebEngineAccessKey::WebEngineAccessKey(QWebEngineView *webEngine, QObject *parent)";
}

WebEngineAccessKey::~WebEngineAccessKey()
{
    delete d;
}

void WebEngineAccessKey::setActionCollection(KActionCollection *ac)
{
    d->mActionCollection = ac;
}

void WebEngineAccessKey::wheelEvent(QWheelEvent *e)
{
    hideAccessKeys();
    if (d->mAccessKeyActivated == WebEngineAccessKeyPrivate::PreActivated && (e->modifiers() & Qt::ControlModifier)) {
        d->mAccessKeyActivated = WebEngineAccessKeyPrivate::NotActivated;
    }
}

void WebEngineAccessKey::resizeEvent(QResizeEvent *)
{
    if (d->mAccessKeyActivated == WebEngineAccessKeyPrivate::Activated) {
        hideAccessKeys();
    }
}

void WebEngineAccessKey::keyPressEvent(QKeyEvent *e)
{
    if (e && d->mWebEngine->hasFocus()) {
        if (d->mAccessKeyActivated == WebEngineAccessKeyPrivate::Activated) {
            if (d->checkForAccessKey(e)) {
                hideAccessKeys();
                e->accept();
                return;
            }
            hideAccessKeys();
        } else if (e->key() == Qt::Key_Control && e->modifiers() == Qt::ControlModifier
#if 0 // FIXME
                   && !isEditableElement(d->mWebView->page())
#endif
        ) {
            d->mAccessKeyActivated = WebEngineAccessKeyPrivate::PreActivated; // Only preactive here, it will be actually activated in key release.
        }
    }
}

void WebEngineAccessKey::keyReleaseEvent(QKeyEvent *e)
{
    // qDebug() << " void WebEngineAccessKey::keyReleaseEvent(QKeyEvent *e)";
    if (d->mAccessKeyActivated == WebEngineAccessKeyPrivate::PreActivated) {
        // Activate only when the CTRL key is pressed and released by itself.
        if (e->key() == Qt::Key_Control && e->modifiers() == Qt::NoModifier) {
            showAccessKeys();
        } else {
            d->mAccessKeyActivated = WebEngineAccessKeyPrivate::NotActivated;
        }
    }
}

void WebEngineAccessKey::hideAccessKeys()
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
        d->mAccessKeyActivated = WebEngineAccessKeyPrivate::NotActivated;
        d->mWebEngine->update();
    }
}

void WebEngineAccessKey::handleSearchAccessKey(const QVariant &res)
{
    // qDebug() << " void WebEngineAccessKey::handleSearchAccessKey(const QVariant &res)" << res;
    const QVariantList lst = res.toList();
    QVector<WebEngineViewer::WebEngineAccessKeyAnchor> anchorList;
    anchorList.reserve(lst.count());
    for (const QVariant &var : lst) {
        // qDebug()<<" var"<<var;
        anchorList << WebEngineViewer::WebEngineAccessKeyAnchor(var);
    }

    QVector<QChar> unusedKeys;
    unusedKeys.reserve(10 + ('Z' - 'A' + 1));
    for (char c = 'A'; c <= 'Z'; ++c) {
        unusedKeys << QLatin1Char(c);
    }
    for (char c = '0'; c <= '9'; ++c) {
        unusedKeys << QLatin1Char(c);
    }
    if (d->mActionCollection) {
        const auto actions = d->mActionCollection->actions();
        for (QAction *act : actions) {
            if (act) {
                const QKeySequence shortCut = act->shortcut();
                if (!shortCut.isEmpty()) {
                    auto lstUnusedKeys = unusedKeys;
                    for (QChar c : std::as_const(unusedKeys)) {
                        if (shortCut.matches(QKeySequence(c)) != QKeySequence::NoMatch) {
                            lstUnusedKeys.removeOne(c);
                        }
                    }
                    unusedKeys = lstUnusedKeys;
                }
            }
        }
    }
    QVector<WebEngineViewer::WebEngineAccessKeyAnchor> unLabeledElements;
    QRect viewport = d->mWebEngine->rect();
    for (const WebEngineViewer::WebEngineAccessKeyAnchor &element : std::as_const(anchorList)) {
        const QRect geometry = element.boundingRect();
        if (geometry.size().isEmpty() || !viewport.contains(geometry.topLeft())) {
            continue;
        }
        if (isHiddenElement(element)) {
            continue; // Do not show access key for hidden elements...
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
            d->makeAccessKeyLabel(accessKey, element);
        }
    }

    // Pick an access key first from the letters in the text and then from the
    // list of unused access keys
    for (const WebEngineViewer::WebEngineAccessKeyAnchor &element : std::as_const(unLabeledElements)) {
        const QRect geometry = element.boundingRect();
        if (unusedKeys.isEmpty() || geometry.size().isEmpty() || !viewport.contains(geometry.topLeft())) {
            continue;
        }
        QChar accessKey;
        const QString text = element.innerText().toUpper();
        for (int i = 0, total = text.count(); i < total; ++i) {
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
            d->makeAccessKeyLabel(accessKey, element);
        }
    }
    d->mAccessKeyActivated = (!d->mAccessKeyLabels.isEmpty() ? WebEngineAccessKeyPrivate::Activated : WebEngineAccessKeyPrivate::NotActivated);
}

void WebEngineAccessKey::showAccessKeys()
{
    d->mAccessKeyActivated = WebEngineAccessKeyPrivate::Activated;
    d->mWebEngine->page()->runJavaScript(WebEngineViewer::WebEngineAccessKeyUtils::script(),
                                         WebEngineManageScript::scriptWordId(),
                                         invokeWebAccessKey(this, &WebEngineAccessKey::handleSearchAccessKey));
}
