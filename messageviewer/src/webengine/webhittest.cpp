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

#include "webhittest.h"
#include "webhittestresult.h"
#include <QWebEnginePage>

using namespace MessageViewer;
template<typename Arg, typename R, typename C>

struct InvokeWrapper
{
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

WebHitTest::WebHitTest(QWebEnginePage *page, const QPoint &pos, QObject *parent)
    : QObject(parent),
      m_pos(pos)
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
    m_pageUrl = page->url();
    page->runJavaScript(js, invoke(this, &WebHitTest::handleHitTest));
}

WebHitTest::~WebHitTest()
{
}

void WebHitTest::handleHitTest(const QVariant &result)
{
    const WebHitTestResult webHitResult(m_pos, m_pageUrl, result);
    Q_EMIT finished(webHitResult);
    deleteLater();
}
