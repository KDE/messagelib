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

QString MailWebEngineScript::manageExpandAddresses(const QString &field)
{
    const QString source = QString::fromLatin1("qt.jQuery('#kmail%1show').click(function(){"
                           "qt.jQuery('#kmail%1show').hide();"
                           "qt.jQuery(\"#kmail%1hide\").show();"
                           "qt.jQuery(\"#dotsFull%1AddressList\").hide();"
                           "qt.jQuery(\"#hiddenFull%1AddressList\").show();}"
                           ");"
                           "qt.jQuery('#kmail%1hide').click(function(){"
                           "qt.jQuery(\"#kmail%1hide\").hide();"
                           "qt.jQuery(\"#kmail%1show\").show();"
                           "qt.jQuery(\"#dotsFull%1AddressList\").show();"
                           "qt.jQuery(\"#hiddenFull%1AddressList\").hide();}"
                           ");").arg(field);
    return source;
}

QString MailWebEngineScript::manageShowHideAttachments()
{
    const QString source = QString::fromLatin1("qt.jQuery('#kmailshowattachment').click(function(){"
                           "qt.jQuery('#kmailshowattachment').hide();"
                           "qt.jQuery(\"#kmailhideattachment\").show();"
                           "qt.jQuery(\"#attachmentid\").hide()}"
                           ");"
                           "qt.jQuery('#kmailhideattachment').click(function(){"
                           "qt.jQuery(\"#kmailhideattachment\").hide();"
                           "qt.jQuery(\"#kmailshowattachment\").show();"
                           "qt.jQuery(\"#attachmentid\").show()}"
                           ");");
    return source;
}

QString MailWebEngineScript::injectAttachments(const QString &delayedHtml, const QString &elementStr)
{
    const QString source = QString::fromLatin1("qt.jQuery('#%1').append('%2')").arg(elementStr).arg(delayedHtml);
    return source;
}

QString MailWebEngineScript::replaceInnerHtml(const QString &field, const QString &html)
{
    const QString replaceInnerHtmlStr = QLatin1String("iconFull") + field + QLatin1String("AddressList");
    const QString source = QString::fromLatin1("qt.jQuery('#%1').append('%2')").arg(replaceInnerHtmlStr).arg(html);
    return source;
}

