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

using namespace MessageViewer;

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


QString WebEngineScript::searchElementPosition(const QString &elementStr)
{
    const QString source = QString::fromLatin1("var element = document.querySelector('%1'); "
                                               "if (element) { "
                                               "    var geometry = element.getBoundingClientRect(); "
                                               "    [(geometry.left + window.scrollX), (geometry.top + window.scrollY)]; "
                                               "}").arg(elementStr);
    qDebug()<<" source "<<source;
    return source;
}
