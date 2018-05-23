/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

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
using namespace MessageViewer;

static QString checkJQuery(const char *scriptName)
{
    return QStringLiteral(
        "if (!qt) { console.warn(\"%1 executed too early, 'qt' variable unknown\"); };\n").arg(QString::fromLatin1(
                                                                                                   scriptName));
}

QString MailWebEngineScript::manageShowHideEncryptionDetails(bool hide)
{
    QString source = checkJQuery("manageShowHideEncryptionDetails");
    if (hide) {
        source += QString::fromLatin1("qt.jQuery(\".enc-details\").hide();"
                                      "qt.jQuery(\".enc-simple\").show();");
    } else {
        source += QString::fromLatin1("qt.jQuery('.enc-simple').hide();"
                                      "qt.jQuery(\".enc-details\").show();");
    }
    return source;
}
