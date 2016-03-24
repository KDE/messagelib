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

#include "mailwebengineaccesskeyutils.h"

QString MessageViewer::MailWebEngineAccessKeyUtils::script()
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
                           "       boundingRect: [r.top, r.left, r.right - r.left, r.bottom - r.top],"
                           "       accessKey: matches[i].getAttribute('accesskey'),"
                           "       target: matches[i].getAttribute('target')"
                           "       });"
                           "}"
                           "return out;})()");
    return script;
}
