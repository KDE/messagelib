/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "defaulttemplates.h"
using namespace Qt::Literals::StringLiterals;

#include <KLocalizedString>

using namespace TemplateParser;

QString DefaultTemplates::defaultNewMessage()
{
    return QLatin1StringView("%REM=\"") + i18n("Default new message template") + QLatin1StringView("\"%-\n") + QLatin1StringView("%BLANK");
}

QString DefaultTemplates::defaultReply()
{
    return u"%REM=\""_s + i18n("Default reply template") + u"\"%-\n"_s
        + i18nc(
               "Default reply template."
               "%1: date of original message, %2: time of original message, "
               "%3: quoted text of original message, %4: cursor Position",
               "On %1 %2 you wrote:\n"
               "%3\n"
               "%4",
               u"%ODATE"_s,
               u"%OTIMELONG"_s,
               u"%QUOTE"_s,
               u"%CURSOR"_s);
}

QString DefaultTemplates::defaultReplyAll()
{
    return u"%REM=\""_s + i18n("Default reply all template") + u"\"%-\n"_s
        + i18nc(
               "Default reply all template: %1: date, %2: time, %3: name of original sender, "
               "%4: quoted text of original message, %5: cursor position",
               "On %1 %2 %3 wrote:\n"
               "%4\n"
               "%5",
               u"%ODATE"_s,
               u"%OTIMELONG"_s,
               u"%OFROMNAME"_s,
               u"%QUOTE"_s,
               u"%CURSOR"_s);
}

QString DefaultTemplates::defaultForward()
{
    return u"%REM=\""_s + i18n("Default forward template") + u"\"%-\n"_s
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
               u"%OFULLSUBJECT"_s,
               u"%ODATE"_s,
               u"%OTIMELONG"_s,
               u"%OFROMADDR"_s,
               u"%TEXT"_s);
}

QString DefaultTemplates::defaultQuoteString()
{
    return u"> "_s;
}
