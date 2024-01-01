/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webhittest.h"
#include "webenginemanagescript.h"
#include "webhittestresult.h"
#include <QWebEnginePage>

using namespace WebEngineViewer;
template<typename Arg, typename R, typename C>

struct InvokeWrapperWebhittest {
    R *receiver;
    void (C::*memberFunction)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFunction)(result);
    }
};

template<typename Arg, typename R, typename C>

InvokeWrapperWebhittest<Arg, R, C> invoke(R *receiver, void (C::*memberFunction)(Arg))
{
    InvokeWrapperWebhittest<Arg, R, C> wrapper = {receiver, memberFunction};
    return wrapper;
}

class WebEngineViewer::WebHitTestPrivate
{
public:
    WebHitTestPrivate(const QPoint &pos, const QPoint &zoomedPos)
        : m_zoomedPos(zoomedPos)
        , m_pos(pos)
    {
    }

    QPoint m_zoomedPos;
    QPoint m_pos;
    QUrl m_pageUrl;
};

WebHitTest::WebHitTest(QWebEnginePage *page, const QPoint &zoomedPos, const QPoint &pos, QObject *parent)
    : QObject(parent)
    , d(new WebHitTestPrivate(pos, zoomedPos))
{
    QString source = QStringLiteral(
        "(function() {"
        "var e = document.elementFromPoint(%1, %2);"
        "if (!e)"
        "    return;"
        "function isMediaElement(e) {"
        "    return e.tagName.toLowerCase() == 'audio' || e.tagName.toLowerCase() == 'video';"
        "}"
        "function isEditableElement(e) {"
        "    if (e.isContentEditable)"
        "        return true;"
        "    if (e.tagName.toLowerCase() == 'input' || e.tagName.toLowerCase() == 'textarea')"
        "        return e.getAttribute('readonly') != 'readonly';"
        "    return false;"
        "}"
        "function isSelected(e) {"
        "    var selection = window.getSelection();"
        "    if (selection.type != 'Range')"
        "        return false;"
        "    return window.getSelection().containsNode(e, true);"
        "}"
        "function attributeStr(e, a) {"
        "    return e.getAttribute(a) || '';"
        "}"
        "var res = {"
        "    alternateText: e.getAttribute('alt'),"
        "    boundingRect: '',"
        "    imageUrl: '',"
        "    contentEditable: isEditableElement(e),"
        "    contentSelected: isSelected(e),"
        "    linkTitle: '',"
        "    linkUrl: '',"
        "    mediaUrl: '',"
        "    tagName: e.tagName.toLowerCase()"
        "};"
        "var r = e.getBoundingClientRect();"
        "res.boundingRect = [r.top, r.left, r.width, r.height];"
        "if (e.tagName.toLowerCase() == 'img')"
        "    res.imageUrl = attributeStr(e, 'src').trim();"
        "if (e.tagName.toLowerCase() == 'a') {"
        "    res.linkTitle = e.text;"
        "    res.linkUrl = attributeStr(e, 'href').trim();"
        "}"
        "while (e) {"
        "    if (res.linkTitle == '' && e.tagName.toLowerCase() == 'a')"
        "        res.linkTitle = e.text;"
        "    if (res.linkUrl == '' && e.tagName.toLowerCase() == 'a')"
        "        res.linkUrl = attributeStr(e, 'href').trim();"
        "    if (res.mediaUrl == '' && isMediaElement(e)) {"
        "        res.mediaUrl = e.currentSrc;"
        "        res.mediaPaused = e.paused;"
        "        res.mediaMuted = e.muted;"
        "    }"
        "    e = e.parentElement;"
        "}"
        "return res;"
        "})()");

    const QString &js = source.arg(d->m_zoomedPos.x()).arg(d->m_zoomedPos.y());
    d->m_pageUrl = page->url();
    page->runJavaScript(js, WebEngineViewer::WebEngineManageScript::scriptWordId(), invoke(this, &WebHitTest::handleHitTest));
}

WebHitTest::~WebHitTest() = default;

void WebHitTest::handleHitTest(const QVariant &result)
{
    const WebHitTestResult webHitResult(d->m_pos, d->m_pageUrl, result);
    Q_EMIT finished(webHitResult);
    deleteLater();
}

#include "moc_webhittest.cpp"
