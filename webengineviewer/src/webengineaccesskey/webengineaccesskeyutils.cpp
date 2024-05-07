/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineaccesskeyutils.h"

QString WebEngineViewer::WebEngineAccessKeyUtils::accessKeyScript()
{
    const QString script = QString::fromLatin1(
        "(function() {"
        "const out = [];"
        "const documents = [...document.querySelectorAll(\"iframe\")].map(iframe => {"
        "  return {"
        "    doc: iframe.contentDocument,"
        "    offset: {x: iframe.getBoundingClientRect().left, y: iframe.getBoundingClientRect().top},"
        "  };"
        "});"

        "documents.push({doc: document, offset: {x: 0, y: 0}});"
        "for (const {doc, offset} of documents) {"
        "  const matches = doc.querySelectorAll(\"a[href], area,button:not([disabled]), "
        "  input:not([disabled]):not([hidden]),label[for],legend,select:not([disabled]),textarea:not([disabled])\");"
        "  for (const match of matches) {"
        "    const r = match.getBoundingClientRect();"
        "    out.push({"
        "      text: match.innerText,"
        "      tagName: match.tagName,"
        "      src: match.href,"
        "      boundingRect: [r.left + offset.x, r.top + offset.y, r.right - r.left, r.bottom - r.top],"
        "      accessKey: match.getAttribute('accesskey'),"
        "      target: match.getAttribute('target')"
        "    });"
        "  }"
        "}"
        "return out;})()");
    return script;
}
