/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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
#include "webenginescript.h"
using namespace WebEngineViewer;

QString WebEngineScript::findAllImages()
{
    const QString source = QStringLiteral("(function() {"
                                          "var out = [];"
                                          "var imgs = document.getElementsByTagName('img');"
                                          "for (var i = 0; i < imgs.length; ++i) {"
                                          "    var e = imgs[i];"
                                          "    out.push({"
                                          "        src: e.src"
                                          "    });"
                                          "}"
                                          "return out;"
                                          "})()");

    return source;
}

QString WebEngineScript::findAllScripts()
{
    const QString source = QStringLiteral("(function() {"
                                          "var out = [];"
                                          "var scripts = document.getElementsByTagName('script');"
                                          "for (var i = 0; i < scripts.length; ++i) {"
                                          "    var e = scripts[i];"
                                          "    out.push({"
                                          "        src: e.src"
                                          "    });"
                                          "}"
                                          "return out;"
                                          "})()");

    return source;
}

QString WebEngineScript::findAllAnchors()
{
    const QString source = QStringLiteral("(function() {"
                                          "var out = [];"
                                          "var anchors = document.getElementsByTagName('a');"
                                          "for (var i = 0; i < anchors.length; ++i) {"
                                          "    var r = anchors[i].getBoundingClientRect();"
                                          "    out.push({"
                                          "        src: anchors[i].href,"
                                          "        title: anchors[i].title,"
                                          "        boudingRect: [r.top, r.left, r.width, r.height]"
                                          "    });"
                                          "}"
                                          "return out;"
                                          "})()");

    return source;
}

QString WebEngineScript::findAllAnchorsAndForms()
{
    const QString source = QStringLiteral("(function() {"
                                          "var res = [];"
                                          "var out = [];"
                                          "var anchors = document.getElementsByTagName('a');"
                                          "for (var i = 0; i < anchors.length; ++i) {"
                                          "    out.push({"
                                          "        src: anchors[i].href,"
                                          "        title: anchors[i].title,"
                                          "        text: anchors[i].innerText"
                                          "    });"
                                          "}"
                                          "var forms = document.getElementsByTagName('form');"
                                          "res.push({"
                                          "    anchors: out,"
                                          "    forms:  forms.length"
                                          "    });"
                                          "return res;"
                                          "})()");

    return source;
}

QString WebEngineScript::setElementByIdVisible(const QString &elementStr, bool visibility)
{
    if (visibility) {
        const QString source = QStringLiteral("var element = document.getElementById('%1'); "
                                              "if (element) { "
                                              "    element.style.removeProperty( 'display' );"
                                              "}").arg(elementStr);
        return source;
    } else {
        const QString source = QStringLiteral("var element = document.getElementById('%1'); "
                                              "if (element) { "
                                              "    element.style.display = \"none\";"
                                              "}").arg(elementStr);
        return source;
    }
}

QString WebEngineScript::searchElementPosition(const QString &elementStr)
{
    const QString source = QStringLiteral("var element = document.getElementById('%1'); "
                                          "if (element) { "
                                          "    var geometry = element.getBoundingClientRect(); "
                                          "    [(geometry.left + window.scrollX), (geometry.top + window.scrollY)]; "
                                          "}").arg(elementStr);
    return source;
}

static QString scrollTop()
{
    return QStringLiteral("document.documentElement.scrollTop");
}

QString WebEngineScript::scrollPercentage(int percent)
{
    const QString source = QStringLiteral("var current = ") + scrollTop() + QStringLiteral(";"
                                                                                           "var docElement = document.documentElement;"
                                                                                           "var height = docElement.clientHeight;"
                                                                                           "var newPosition = current + height * %1 /100;"
                                                                                           "window.scrollTo(window.scrollX, newPosition);").arg(percent);
    return source;
}

QString WebEngineScript::scrollUp(int pixel)
{
    const QString source = QStringLiteral("window.scrollBy(0, %1);").arg(-pixel);
    return source;
}

QString WebEngineScript::scrollDown(int pixel)
{
    const QString source = QStringLiteral("window.scrollBy(0, %1);").arg(pixel);
    return source;
}

QString WebEngineScript::scrollToPosition(const QPoint &pos)
{
    const QString source = QStringLiteral("window.scrollTo(%1, %2); [window.scrollX, window.scrollY];").arg(pos.x()).arg(pos.y());
    return source;
}

QString WebEngineScript::removeStyleToElement(const QString &elementStr)
{
    const QString source = QStringLiteral("var element = document.getElementById('%1'); "
                                          "if (element) { "
                                          "    element.removeAttribute(\"style\");"
                                          "}").arg(elementStr);
    return source;
}

QString WebEngineScript::setStyleToElement(const QString &elementStr, const QString &style)
{
    const QString source = QStringLiteral("var element = document.getElementById('%1'); "
                                          "if (element) { "
                                          "    element.style = '%2';"
                                          "}").arg(elementStr, style);
    return source;
}

QString WebEngineScript::scrollToRelativePosition(qreal pos)
{
    const QString source = QStringLiteral("window.scrollTo(window.scrollX, %1); [window.scrollX, window.scrollY];").arg(pos);
    return source;
}

QString WebEngineScript::isScrolledToBottom()
{
    return QStringLiteral("(function() { "
                          "var docElement = document.documentElement;"
                          "var viewportHeight = docElement.clientHeight;"
                          "var isAtBottom = ") + scrollTop() + QStringLiteral(" + viewportHeight >= document.body.scrollHeight;"
                                                                              "return Boolean(isAtBottom); "
                                                                              "}());");
}
