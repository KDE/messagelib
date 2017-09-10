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

#include "webhittest.h"
#include "webhittestresult.h"
#include <QWebEnginePage>
#include "webenginemanagescript.h"

using namespace WebEngineViewer;
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

class WebEngineViewer::WebHitTestPrivate
{
public:
    WebHitTestPrivate(const QPoint &pos)
        : m_pos(pos)
    {
    }

    QPoint m_pos;
    QUrl m_pageUrl;
};

WebHitTest::WebHitTest(QWebEnginePage *page, const QPoint &pos, QObject *parent)
    : QObject(parent)
    , d(new WebHitTestPrivate(pos))
{
    QString source = QStringLiteral("(function() {"
                                    "var e = document.elementFromPoint(%1, %2);"
                                    "if (!e)"
                                    "    return;"
                                    "function isMediaElement(e) {"
                                    "    return e.tagName == 'AUDIO' || e.tagName == 'VIDEO';"
                                    "}"
                                    "function isEditableElement(e) {"
                                    "    if (e.isContentEditable)"
                                    "        return true;"
                                    "    if (e.tagName == 'INPUT' || e.tagName == 'TEXTAREA')"
                                    "        return e.getAttribute('readonly') != 'readonly';"
                                    "    return false;"
                                    "}"
                                    "function isSelected(e) {"
                                    "    var selection = window.getSelection();"
                                    "    if (selection.type != 'Range')"
                                    "        return false;"
                                    "    return window.getSelection().containsNode(e, true);"
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
                                    "if (e.tagName == 'IMG')"
                                    "    res.imageUrl = e.getAttribute('src');"
                                    "if (e.tagName == 'A') {"
                                    "    res.linkTitle = e.text;"
                                    "    res.linkUrl = e.getAttribute('href');"
                                    "}"
                                    "while (e) {"
                                    "    if (res.linkTitle == '' && e.tagName == 'A')"
                                    "        res.linkTitle = e.text;"
                                    "    if (res.linkUrl == '' && e.tagName == 'A')"
                                    "        res.linkUrl = e.getAttribute('href');"
                                    "    if (res.mediaUrl == '' && isMediaElement(e)) {"
                                    "        res.mediaUrl = e.currentSrc;"
                                    "        res.mediaPaused = e.paused;"
                                    "        res.mediaMuted = e.muted;"
                                    "    }"
                                    "    e = e.parentElement;"
                                    "}"
                                    "return res;"
                                    "})()");

    const QString &js = source.arg(pos.x()).arg(pos.y());
    d->m_pageUrl = page->url();
    page->runJavaScript(js,
                        WebEngineViewer::WebEngineManageScript::scriptWordId(),
                        invoke(this, &WebHitTest::handleHitTest));
}

WebHitTest::~WebHitTest()
{
    delete d;
}

void WebHitTest::handleHitTest(const QVariant &result)
{
    const WebHitTestResult webHitResult(d->m_pos, d->m_pageUrl, result);
    Q_EMIT finished(webHitResult);
    deleteLater();
}
