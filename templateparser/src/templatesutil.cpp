/*
  SPDX-FileCopyrightText: 2011-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templatesutil.h"

#include "templatesutil_p.h"

#include <KConfigGroup>
#include <KEmailAddress>
#include <KSharedConfig>
#include <QRegularExpression>
#include <QStringList>

using namespace Qt::Literals::StringLiterals;
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
        << u"%REM=\"\"%-"_s << u"%INSERT=\"\""_s << u"%SYSTEM=\"\""_s << u"%QUOTEPIPE=\"\""_s << u"%MSGPIPE=\"\""_s << u"%BODYPIPE=\"\""_s
        << u"%CLEARPIPE=\"\""_s << u"%TEXTPIPE=\"\""_s << u"%OHEADER=\"\""_s << u"%HEADER=\"\""_s << u"%DICTIONARYLANGUAGE=\"\""_s << u"%LANGUAGE=\"\""_s;
    return keywordsWithArgs;
}

QStringList TemplateParser::Util::keywords()
{
    const QStringList keywords =
        QStringList() << u"%QUOTE"_s << u"%FORCEDPLAIN"_s << u"%FORCEDHTML"_s << u"%QHEADERS"_s << u"%HEADERS"_s << u"%TEXT"_s << u"%OTEXTSIZE"_s << u"%OTEXT"_s
                      << u"%OADDRESSEESADDR"_s << u"%CCADDR"_s << u"%CCNAME"_s << u"%CCFNAME"_s << u"%CCLNAME"_s << u"%TOADDR"_s << u"%TONAME"_s
                      << u"%TOFNAME"_s << u"%TOLNAME"_s << u"%TOLIST"_s << u"%FROMADDR"_s << u"%FROMNAME"_s << u"%FROMFNAME"_s << u"%FROMLNAME"_s
                      << u"%FULLSUBJECT"_s << u"%MSGID"_s << u"%HEADER\\( "_s << u"%OCCADDR"_s << u"%OCCNAME"_s << u"%OCCFNAME"_s << u"%OCCLNAME"_s
                      << u"%OTOADDR"_s << u"%OTONAME"_s << u"%OTOFNAME"_s << u"%OTOLNAME"_s << u"%OTOLIST"_s << u"%OTO"_s << u"%OFROMADDR"_s << u"%OFROMNAME"_s
                      << u"%OFROMFNAME"_s << u"%OFROMLNAME"_s << u"%OFULLSUBJECT"_s << u"%OFULLSUBJ"_s << u"%OMSGID"_s << u"%DATEEN"_s << u"%DATESHORT"_s
                      << u"%DATE"_s << u"%DOW"_s << u"%TIMELONGEN"_s << u"%TIMELONG"_s << u"%TIME"_s << u"%ODATEEN"_s << u"%ODATESHORT"_s << u"%ODATE"_s
                      << u"%ODOW"_s << u"%OTIMELONGEN"_s << u"%OTIMELONG"_s << u"%OTIME"_s << u"%BLANK"_s << u"%NOP"_s << u"%CLEAR"_s << u"%DEBUGOFF"_s
                      << u"%DEBUG"_s << u"%CURSOR"_s << u"%SIGNATURE"_s;
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
        if ((sep_pos = name.indexOf(u',')) < 0) {
            // no comma, start at the beginning of the string and return the first sequence
            // of non-whitespace characters
            for (int i = 0; i < nameLength; i++) {
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
        if ((sep_pos = name.indexOf(u',')) < 0) {
            // no comma, start at the end of the string and return the last sequence
            // of non-whitespace characters
            for (int i = nameLength - 1; i >= 0; i--) {
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
