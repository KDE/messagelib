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
#include "webenginescript.h"
#include <QDebug>

using namespace WebEngineViewer;

QString WebEngineScript::findAllImages()
{
    const QString source = QLatin1String("(function() {"
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
    const QString source = QLatin1String("(function() {"
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
    const QString source = QLatin1String("(function() {"
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
    const QString source = QLatin1String("(function() {"
                                         "var res = [];"
                                         "var out = [];"
                                         "var anchors = document.getElementsByTagName('a');"
                                         "for (var i = 0; i < anchors.length; ++i) {"
                                         "    out.push({"
                                         "        src: anchors[i].href,"
                                         "        title: anchors[i].title"
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
        const QString source = QString::fromLatin1("var element = document.getElementById('%1'); "
                               "if (element) { "
                               "    element.style.removeProperty( 'display' );"
                               "}").arg(elementStr);
        return source;
    } else {
        const QString source = QString::fromLatin1("var element = document.getElementById('%1'); "
                               "if (element) { "
                               "    element.style.display = \"none\";"
                               "}").arg(elementStr);
        return source;
    }
}

QString WebEngineScript::searchElementPosition(const QString &elementStr)
{
    const QString source = QString::fromLatin1("var element = document.getElementById('%1'); "
                           "if (element) { "
                           "    var geometry = element.getBoundingClientRect(); "
                           "    [(geometry.left + window.scrollX), (geometry.top + window.scrollY)]; "
                           "}").arg(elementStr);
    //qDebug()<<" source "<<source;
    return source;
}

QString WebEngineScript::scrollPercentage(int percent)
{
    const QString source = QString::fromLatin1("var current = document.body.scrollTop;"
                           "var docElement = document.documentElement;"
                           "var height = docElement.clientHeight;"
                           "var newPosition = current + height * %1 /100;"
                           "window.scrollTo(window.scrollX, newPosition);").arg(percent);

    //qDebug() << "QString WebEngineScript::scrollPercentage(int percent) " << source;
    return source;
}

QString WebEngineScript::scrollUp(int pixel)
{
    const QString source = QString::fromLatin1("window.scrollBy(0, %1);").arg(-pixel);
    return source;
}

QString WebEngineScript::scrollDown(int pixel)
{
    const QString source = QString::fromLatin1("window.scrollBy(0, %1);").arg(pixel);
    return source;
}

QString WebEngineScript::scrollToPosition(const QPoint &pos)
{
    const QString source = QString::fromLatin1("window.scrollTo(%1, %2); [window.scrollX, window.scrollY];").arg(pos.x()).arg(pos.y());
    //qDebug()<<" source "<<source;
    return source;
}

QString WebEngineScript::removeStyleToElement(const QString &elementStr)
{
    const QString source = QString::fromLatin1("var element = document.getElementById('%1'); "
                           "if (element) { "
                           "    element.removeAttribute(\"style\");"
                           "}").arg(elementStr);
    return source;
}

QString WebEngineScript::setStyleToElement(const QString &elementStr, const QString &style)
{
    const QString source = QString::fromLatin1("var element = document.getElementById('%1'); "
                           "if (element) { "
                           "    element.style = '%2';"
                           "}").arg(elementStr).arg(style);
    //qDebug() << "QString WebEngineScript::setStyleToElement(const QString &elementStr, const QString &style) " << source;
    return source;
}

QString WebEngineScript::scrollToRelativePosition(qreal pos)
{
    //TODO verify it
    const QString source = QString::fromLatin1("window.scrollTo(window.scrollX, %1); [window.scrollX, window.scrollY];").arg(pos);
    return source;
}

QString WebEngineScript::clearSelection()
{
#if 0
    if (window.getSelection) {
        if (window.getSelection().empty) {  // Chrome
            window.getSelection().empty();
        } else if (window.getSelection().removeAllRanges) {  // Firefox
            window.getSelection().removeAllRanges();
        }
    }
#endif
    const QString source = QString::fromLatin1("(function() {"
                           "if (window.getSelection) {"
                           "  if (window.getSelection().empty) {"
                           "    window.getSelection().empty();"
                           "  }"
                           "}"
                           "})()");
    //qDebug() << " source " << source;
    return source;
}
