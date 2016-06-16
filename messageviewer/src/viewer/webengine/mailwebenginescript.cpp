/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "mailwebenginescript.h"
#include "webengineviewer/webenginescript.h"
#include <QDebug>
using namespace MessageViewer;

QString MailWebEngineScript::injectAttachments(const QString &delayedHtml, const QString &elementStr)
{
    const QString source = QString::fromLatin1("var element = document.getElementById('%1'); "
                           "if (element) { "
                           "    element.innerHTML += '%2';"
                           "}").arg(elementStr).arg(delayedHtml);
    qDebug() << "QString MailWebEngineScript::injectAttachments(const QString &delayedHtml, const QString &elementStr) :" << source;
    return source;
}

QString MailWebEngineScript::replaceInnerHtml(const QString &field, const QString &html, bool doShow)
{
    const QString replaceInnerHtmlStr = QLatin1String("iconFull") + field + QLatin1String("AddressList");
    const QString source = QString::fromLatin1("(function() {"
                           "var doShow = %3;"
                           "var field = \'%4\';"
                           "var out = [];"
                           "var element = document.getElementById('%1'); "
                           "if (element) { "
                           "    element.innerHTML = '%2';"
                           "    out.push({"
                           "        field: field,"
                           "        doShow: doShow"
                           "    });"
                           "}"
                           "return out;"
                           "})()").arg(replaceInnerHtmlStr).arg(html).arg(doShow).arg(field);
    qDebug() << "QString MailWebEngineScript::replaceInnerHtml(const QString &delayedHtml, const QString &elementStr) :" << source;
    return source;

}

QString MailWebEngineScript::updateToggleFullAddressList(const QString &field, bool doShow)
{
    const QString dotsFullStr = QLatin1String("dotsFull")   + field + QLatin1String("AddressList");
    const QString hiddenFullStr = QLatin1String("hiddenFull") + field + QLatin1String("AddressList");
    const QString source = QString::fromLatin1("    %1;"
                           "    %2;")
                           .arg(WebEngineViewer::WebEngineScript::setElementByIdVisible(dotsFullStr, !doShow))
                           .arg(WebEngineViewer::WebEngineScript::setElementByIdVisible(hiddenFullStr, doShow));
    qDebug() << "QString MailWebEngineScript::updateToggleFullAddressList(const QString &delayedHtml, const QString &elementStr) :" << source;
    return source;

}

QString MailWebEngineScript::toggleFullAddressList(const QString &field, const QString &html, bool doShow)
{
    const QString replaceInnerHtmlStr = QLatin1String("iconFull") + field + QLatin1String("AddressList");
    const QString dotsFullStr = QLatin1String("dotsFull")   + field + QLatin1String("AddressList");
    const QString hiddenFullStr = QLatin1String("hiddenFull") + field + QLatin1String("AddressList");
    const QString source = QString::fromLatin1("var element = document.getElementById('%1'); "
                           "if (element) { "
                           "    element.innerHTML = '%2';"
                           "    %3;"
                           "    %4;"
                           "}").arg(replaceInnerHtmlStr).arg(html)
                           .arg(WebEngineViewer::WebEngineScript::setElementByIdVisible(dotsFullStr, !doShow))
                           .arg(WebEngineViewer::WebEngineScript::setElementByIdVisible(hiddenFullStr, doShow));
    qDebug() << "QString MailWebEngineScript::injectAttachments(const QString &delayedHtml, const QString &elementStr) :" << source;
    return source;
}

