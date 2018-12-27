/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

#include "webengineaccesskeyutils.h"

QString WebEngineViewer::WebEngineAccessKeyUtils::script()
{
    const QString script = QString::fromLatin1("(function() {"
                                               "var out = [];"
                                               "var matches = document.querySelectorAll(\"a[href], area,button:not([disabled]), input:not([disabled]):not([hidden]),label[for],legend,select:not([disabled]),textarea:not([disabled])\");"
                                               "for (var i = 0; i < matches.length; ++i) {"
                                               "     var r = matches[i].getBoundingClientRect();"
                                               "     out.push({"
                                               "       text: matches[i].innerText,"
                                               "       tagName: matches[i].tagName,"
                                               "       src: matches[i].href,"
                                               "       boundingRect: [r.left, r.top, r.right - r.left, r.bottom - r.top],"
                                               "       accessKey: matches[i].getAttribute('accesskey'),"
                                               "       target: matches[i].getAttribute('target')"
                                               "       });"
                                               "}"
                                               "return out;})()");
    return script;
}
