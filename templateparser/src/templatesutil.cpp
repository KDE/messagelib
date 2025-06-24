/*
  SPDX-FileCopyrightText: 2011-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templatesutil.h"
using namespace Qt::Literals::StringLiterals;

#include "templatesutil_p.h"

#include <KConfigGroup>
#include <KEmailAddress>
#include <KSharedConfig>
#include <QRegularExpression>
#include <QStringList>

using namespace TemplateParser;

void TemplateParser::Util::deleteTemplate(const QString &id)
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig(u"templatesconfigurationrc"_s, KConfig::NoGlobals);

    const QString key = u"Templates #%1"_s.arg(id);
    if (config->hasGroup(key)) {
        KConfigGroup group = config->group(key);
        group.deleteGroup();
        group.sync();
    }
}

QStringList TemplateParser::Util::keywordsWithArgs()
{
    const QStringList keywordsWithArgs = QStringList()
        << u"%REM=\"\"%-"_s << u"%INSERT=\"\""_s << QStringLiteral("%SYSTEM=\"\"") << QStringLiteral("%QUOTEPIPE=\"\"") << u"%MSGPIPE=\"\""_s
        << u"%BODYPIPE=\"\""_s << QStringLiteral("%CLEARPIPE=\"\"") << QStringLiteral("%TEXTPIPE=\"\"") << u"%OHEADER=\"\""_s << u"%HEADER=\"\""_s
        << QStringLiteral("%DICTIONARYLANGUAGE=\"\"") << QStringLiteral("%LANGUAGE=\"\"");
    return keywordsWithArgs;
}

QStringList TemplateParser::Util::keywords()
{
    const QStringList keywords =
        QStringList() << u"%QUOTE"_s << u"%FORCEDPLAIN"_s << QStringLiteral("%FORCEDHTML") << QStringLiteral("%QHEADERS") << u"%HEADERS"_s << u"%TEXT"_s
                      << QStringLiteral("%OTEXTSIZE") << QStringLiteral("%OTEXT") << u"%OADDRESSEESADDR"_s << u"%CCADDR"_s << QStringLiteral("%CCNAME")
                      << QStringLiteral("%CCFNAME") << u"%CCLNAME"_s << u"%TOADDR"_s << QStringLiteral("%TONAME") << QStringLiteral("%TOFNAME") << u"%TOLNAME"_s
                      << u"%TOLIST"_s << QStringLiteral("%FROMADDR") << QStringLiteral("%FROMNAME") << u"%FROMFNAME"_s << u"%FROMLNAME"_s
                      << QStringLiteral("%FULLSUBJECT") << QStringLiteral("%MSGID") << u"%HEADER\\( "_s << u"%OCCADDR"_s << QStringLiteral("%OCCNAME")
                      << QStringLiteral("%OCCFNAME") << u"%OCCLNAME"_s << u"%OTOADDR"_s << QStringLiteral("%OTONAME") << QStringLiteral("%OTOFNAME")
                      << u"%OTOLNAME"_s << u"%OTOLIST"_s << QStringLiteral("%OTO") << QStringLiteral("%OFROMADDR") << u"%OFROMNAME"_s << u"%OFROMFNAME"_s
                      << QStringLiteral("%OFROMLNAME") << QStringLiteral("%OFULLSUBJECT") << u"%OFULLSUBJ"_s << u"%OMSGID"_s << QStringLiteral("%DATEEN")
                      << QStringLiteral("%DATESHORT") << u"%DATE"_s << u"%DOW"_s << QStringLiteral("%TIMELONGEN") << QStringLiteral("%TIMELONG") << u"%TIME"_s
                      << u"%ODATEEN"_s << QStringLiteral("%ODATESHORT") << QStringLiteral("%ODATE") << u"%ODOW"_s << u"%OTIMELONGEN"_s
                      << QStringLiteral("%OTIMELONG") << QStringLiteral("%OTIME") << u"%BLANK"_s << u"%NOP"_s << QStringLiteral("%CLEAR")
                      << QStringLiteral("%DEBUGOFF") << u"%DEBUG"_s << u"%CURSOR"_s << QStringLiteral("%SIGNATURE");
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
        if ((sep_pos = name.indexOf(u',')) < 0) {
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
        if ((sep_pos = mail.indexOf(u'@')) < 0) {
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
        if ((sep_pos = name.indexOf(u',')) < 0) {
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
    const int firstNonWS = content.indexOf(QRegularExpression(u"\\S"_s));
    const int lineStart = content.lastIndexOf(u'\n', firstNonWS);
    if (lineStart >= 0) {
        content.remove(0, lineStart);
    }
    return content;
}
