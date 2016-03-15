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
                                         "var anchor = document.getElementsByTagName('a');"
                                         "for (var i = 0; i < anchor.length; ++i) {"
                                         "    var e = anchor[i];"
                                         "    out.push({"
                                         "        src: e.getAttribute('href');"
                                         "        title = e.text;"
                                         "    });"
                                         "}"
                                         "return out;"
                                         "})()");

    return source;
}
