/*
  SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templatesutil.h"
#include "templatesutil_p.h"

#include <KConfigGroup>
#include <KEmailAddress>
#include <KSharedConfig>
#include <QRegularExpression>
#include <QStringList>

using namespace TemplateParser;

void TemplateParser::Util::deleteTemplate(const QString &id)
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral("templatesconfigurationrc"), KConfig::NoGlobals);

    const QString key = QStringLiteral("Templates #%1").arg(id);
    if (config->hasGroup(key)) {
        KConfigGroup group = config->group(key);
        group.deleteGroup();
        group.sync();
    }
}

QStringList TemplateParser::Util::keywordsWithArgs()
{
    const QStringList keywordsWithArgs = QStringList()
        << QStringLiteral("%REM=\"\"%-") << QStringLiteral("%INSERT=\"\"") << QStringLiteral("%SYSTEM=\"\"") << QStringLiteral("%QUOTEPIPE=\"\"")
        << QStringLiteral("%MSGPIPE=\"\"") << QStringLiteral("%BODYPIPE=\"\"") << QStringLiteral("%CLEARPIPE=\"\"") << QStringLiteral("%TEXTPIPE=\"\"")
        << QStringLiteral("%OHEADER=\"\"") << QStringLiteral("%HEADER=\"\"") << QStringLiteral("%DICTIONARYLANGUAGE=\"\"") << QStringLiteral("%LANGUAGE=\"\"");
    return keywordsWithArgs;
}

QStringList TemplateParser::Util::keywords()
{
    const QStringList keywords =
        QStringList() << QStringLiteral("%QUOTE") << QStringLiteral("%FORCEDPLAIN") << QStringLiteral("%FORCEDHTML") << QStringLiteral("%QHEADERS")
                      << QStringLiteral("%HEADERS") << QStringLiteral("%TEXT") << QStringLiteral("%OTEXTSIZE") << QStringLiteral("%OTEXT")
                      << QStringLiteral("%OADDRESSEESADDR") << QStringLiteral("%CCADDR") << QStringLiteral("%CCNAME") << QStringLiteral("%CCFNAME")
                      << QStringLiteral("%CCLNAME") << QStringLiteral("%TOADDR") << QStringLiteral("%TONAME") << QStringLiteral("%TOFNAME")
                      << QStringLiteral("%TOLNAME") << QStringLiteral("%TOLIST") << QStringLiteral("%FROMADDR") << QStringLiteral("%FROMNAME")
                      << QStringLiteral("%FROMFNAME") << QStringLiteral("%FROMLNAME") << QStringLiteral("%FULLSUBJECT") << QStringLiteral("%MSGID")
                      << QStringLiteral("%HEADER\\( ") << QStringLiteral("%OCCADDR") << QStringLiteral("%OCCNAME") << QStringLiteral("%OCCFNAME")
                      << QStringLiteral("%OCCLNAME") << QStringLiteral("%OTOADDR") << QStringLiteral("%OTONAME") << QStringLiteral("%OTOFNAME")
                      << QStringLiteral("%OTOLNAME") << QStringLiteral("%OTOLIST") << QStringLiteral("%OTO") << QStringLiteral("%OFROMADDR")
                      << QStringLiteral("%OFROMNAME") << QStringLiteral("%OFROMFNAME") << QStringLiteral("%OFROMLNAME") << QStringLiteral("%OFULLSUBJECT")
                      << QStringLiteral("%OFULLSUBJ") << QStringLiteral("%OMSGID") << QStringLiteral("%DATEEN") << QStringLiteral("%DATESHORT")
                      << QStringLiteral("%DATE") << QStringLiteral("%DOW") << QStringLiteral("%TIMELONGEN") << QStringLiteral("%TIMELONG")
                      << QStringLiteral("%TIME") << QStringLiteral("%ODATEEN") << QStringLiteral("%ODATESHORT") << QStringLiteral("%ODATE")
                      << QStringLiteral("%ODOW") << QStringLiteral("%OTIMELONGEN") << QStringLiteral("%OTIMELONG") << QStringLiteral("%OTIME")
                      << QStringLiteral("%BLANK") << QStringLiteral("%NOP") << QStringLiteral("%CLEAR") << QStringLiteral("%DEBUGOFF")
                      << QStringLiteral("%DEBUG") << QStringLiteral("%CURSOR") << QStringLiteral("%SIGNATURE");
    return keywords;
}

QString TemplateParser::Util::getFirstNameFromEmail(const QString &str)
{
    // simple logic:
    // 1. If there is ',' in name, than format is 'Last, First'. If the first name consists
    //    of several words, all parts are returned.
    // 2. If there is no ',' in the name, the format is 'First Last'. If the first name consists
    //    of several words, there is not way to decided whether the middle names are part of the
    //    the first or the last name, so we return only the first word.
    // 3. If the display name is empty, return 'name' from 'name@domain'.

    QString res;
    QString mail, name;
    KEmailAddress::extractEmailAddressAndName(str, mail, name);
    if (!name.isEmpty()) {
        // we have a display name, look for a comma
        int nameLength = name.length();
        int sep_pos = -1;
        int i;
        if ((sep_pos = name.indexOf(QLatin1Char(','))) < 0) {
            // no comma, start at the beginning of the string and return the first sequence
            // of non-whitespace characters
            for (i = 0; i < nameLength; i++) {
                const QChar c = name.at(i);
                if (!c.isSpace()) {
                    res.append(c);
                } else {
                    break;
                }
            }
        } else {
            // found a comma, first name is everything after that comma
            res = QStringView(name).mid(sep_pos + 1).trimmed().toString();
        }
    } else if (!mail.isEmpty()) {
        // extract the part of the mail address before the '@'
        int sep_pos = -1;
        if ((sep_pos = mail.indexOf(QLatin1Char('@'))) < 0) {
            // no '@', this should actually never happen, but just in case we return the
            // full address
            sep_pos = mail.length();
        }
        res = mail.left(sep_pos);
    }

    return res;
}

QString TemplateParser::Util::getLastNameFromEmail(const QString &str)
{
    // simple logic:
    // 1. If there is ',' in name, than format is 'Last, First'. If the last name consists
    //    of several words, all parts are returned (i.e. everything before the ',' is returned).
    // 2. If there is no ',' in the name, the format is 'First Last'. If the last name consists
    //    of several words, there is not way to decided whether the middle names are part of the
    //    the first or the last name, so we return only the last word.

    QString res;
    QString mail, name;
    KEmailAddress::extractEmailAddressAndName(str, mail, name);
    if (!name.isEmpty()) {
        // we have a display name, look for a comma
        int nameLength = name.length();
        int sep_pos = -1;
        int i;
        if ((sep_pos = name.indexOf(QLatin1Char(','))) < 0) {
            // no comma, start at the end of the string and return the last sequence
            // of non-whitespace characters
            for (i = nameLength - 1; i >= 0; i--) {
                const QChar c = name.at(i);
                if (!c.isSpace()) {
                    res.prepend(c);
                } else {
                    break;
                }
            }
        } else {
            // found a comma, last name is everything before that comma
            res = QStringView(name).left(sep_pos).trimmed().toString();
        }
    }

    return res;
}

QString TemplateParser::Util::removeSpaceAtBegin(const QString &selection)
{
    QString content = selection;
    // Remove blank lines at the beginning:
    const int firstNonWS = content.indexOf(QRegularExpression(QStringLiteral("\\S")));
    const int lineStart = content.lastIndexOf(QLatin1Char('\n'), firstNonWS);
    if (lineStart >= 0) {
        content.remove(0, lineStart);
    }
    return content;
}
