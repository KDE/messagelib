/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "defaulttemplates.h"

#include <KLocalizedString>

using namespace TemplateParser;

QString DefaultTemplates::defaultNewMessage()
{
    return QLatin1StringView("%REM=\"") + i18n("Default new message template") + QLatin1StringView("\"%-\n") + QLatin1StringView("%BLANK");
}

QString DefaultTemplates::defaultReply()
{
    return QStringLiteral("%REM=\"") + i18n("Default reply template") + QStringLiteral("\"%-\n")
        + i18nc(
               "Default reply template."
               "%1: date of original message, %2: time of original message, "
               "%3: quoted text of original message, %4: cursor Position",
               "On %1 %2 you wrote:\n"
               "%3\n"
               "%4",
               QStringLiteral("%ODATE"),
               QStringLiteral("%OTIMELONG"),
               QStringLiteral("%QUOTE"),
               QStringLiteral("%CURSOR"));
}

QString DefaultTemplates::defaultReplyAll()
{
    return QStringLiteral("%REM=\"") + i18n("Default reply all template") + QStringLiteral("\"%-\n")
        + i18nc(
               "Default reply all template: %1: date, %2: time, %3: name of original sender, "
               "%4: quoted text of original message, %5: cursor position",
               "On %1 %2 %3 wrote:\n"
               "%4\n"
               "%5",
               QStringLiteral("%ODATE"),
               QStringLiteral("%OTIMELONG"),
               QStringLiteral("%OFROMNAME"),
               QStringLiteral("%QUOTE"),
               QStringLiteral("%CURSOR"));
}

QString DefaultTemplates::defaultForward()
{
    return QStringLiteral("%REM=\"") + i18n("Default forward template") + QStringLiteral("\"%-\n")
        + i18nc(
               "Default forward template: %1: subject of original message, "
               "%2: date of original message, "
               "%3: time of original message, "
               "%4: mail address of original sender, "
               "%5: original message text",
               "\n"
               "----------  Forwarded Message  ----------\n"
               "\n"
               "Subject: %1\n"
               "Date: %2, %3\n"
               "From: %4\n"
               "%OADDRESSEESADDR\n"
               "\n"
               "%5\n"
               "-----------------------------------------",
               QStringLiteral("%OFULLSUBJECT"),
               QStringLiteral("%ODATE"),
               QStringLiteral("%OTIMELONG"),
               QStringLiteral("%OFROMADDR"),
               QStringLiteral("%TEXT"));
}

QString DefaultTemplates::defaultQuoteString()
{
    return QStringLiteral("> ");
}
