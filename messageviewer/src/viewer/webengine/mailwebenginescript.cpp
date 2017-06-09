/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

static QString checkJQuery(const char *scriptName)
{
    return QStringLiteral(
        "if (!qt) { console.warn(\"%1 executed too early, 'qt' variable unknown\"); };\n").arg(QString::fromLatin1(
                                                                                                   scriptName));
}

QString MailWebEngineScript::createShowHideAddressScript(const QString &field, bool hide)
{
    QString source = checkJQuery("createShowHideAddressScript");
    if (hide) {
        source += QString::fromLatin1("qt.jQuery(\"#kmail%1show\").hide();"
                                      "qt.jQuery(\"#kmail%1hide\").show();"
                                      "qt.jQuery(\"#dotsFull%1AddressList\").hide();"
                                      "qt.jQuery(\"#hiddenFull%1AddressList\").show();").arg(field);
    } else {
        source += QString::fromLatin1("qt.jQuery(\"#kmail%1hide\").hide();"
                                      "qt.jQuery(\"#kmail%1show\").show();"
                                      "qt.jQuery(\"#dotsFull%1AddressList\").show();"
                                      "qt.jQuery(\"#hiddenFull%1AddressList\").hide();").arg(field);
    }
    return source;
}

QString MailWebEngineScript::manageShowHideToAddress(bool hide)
{
    return MailWebEngineScript::createShowHideAddressScript(QStringLiteral("To"), hide);
}

QString MailWebEngineScript::manageShowHideCcAddress(bool hide)
{
    return MailWebEngineScript::createShowHideAddressScript(QStringLiteral("Cc"), hide);
}

QString MailWebEngineScript::manageShowHideAttachments(bool hide)
{
    QString source = checkJQuery("manageShowHideAttachments");
    if (hide) {
        source += QString::fromLatin1("qt.jQuery(\"#kmailhideattachment\").hide();"
                                      "qt.jQuery(\"#kmailshowattachment\").show();"
                                      "if (!qt.jQuery(\"#attachmentlist\")) { console.warn('attachmentlist not found'); } else { qt.jQuery(\"#attachmentlist\").show(); }");
    } else {
        source += QString::fromLatin1("qt.jQuery('#kmailshowattachment').hide();"
                                      "qt.jQuery(\"#kmailhideattachment\").show();"
                                      "if (!qt.jQuery(\"#attachmentlist\")) { console.warn('attachmentlist not found'); } else { qt.jQuery(\"#attachmentlist\").hide(); }");
    }
    return source;
}

QString MailWebEngineScript::injectAttachments(const QString &delayedHtml,
                                               const QString &elementStr)
{
    const QString source = checkJQuery("injectAttachments") + QString::fromLatin1(
        "if (!document.getElementById('%1')) { console.warn('NOT FOUND: %1'); };\n"
        "qt.jQuery('#%1').append('%2')").arg(elementStr, delayedHtml);
    return source;
}

QString MailWebEngineScript::replaceInnerHtml(const QString &field, const QString &html)
{
    const QString replaceInnerHtmlStr = QLatin1String("iconFull") + field + QLatin1String(
        "AddressList");
    const QString source = checkJQuery("replaceInnerHtml") + QString::fromLatin1(
        "qt.jQuery('#%1').append('%2')").arg(replaceInnerHtmlStr, html);
    return source;
}
