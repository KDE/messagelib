/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineaccesskeyutils.h"
using namespace Qt::Literals::StringLiterals;
QString WebEngineViewer::WebEngineAccessKeyUtils::accessKeyScript()
{
    const QString script =
        u"(function() {"
        "var out = [];"
        "var matches = document.querySelectorAll(\"a[href], area,button:not([disabled]), "
        "input:not([disabled]):not([hidden]),label[for],legend,select:not([disabled]),textarea:not([disabled])\");"
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
        "return out;})()"_s;
    return script;
}
