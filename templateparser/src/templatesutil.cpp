/*
  Copyright (c) 2011-2019 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "templatesutil.h"
#include "templatesutil_p.h"

#include <KConfigGroup>
#include <KSharedConfig>
#include <QRegularExpression>
#include <QStringList>
using namespace TemplateParser;

void TemplateParser::Util::deleteTemplate(const QString &id)
{
    KSharedConfig::Ptr config
        = KSharedConfig::openConfig(QStringLiteral("templatesconfigurationrc"), KConfig::NoGlobals);

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
                                         << QStringLiteral("%REM=\"\"%-")
                                         << QStringLiteral("%INSERT=\"\"")
                                         << QStringLiteral("%SYSTEM=\"\"")
                                         << QStringLiteral("%QUOTEPIPE=\"\"")
                                         << QStringLiteral("%MSGPIPE=\"\"")
                                         << QStringLiteral("%BODYPIPE=\"\"")
                                         << QStringLiteral("%CLEARPIPE=\"\"")
                                         << QStringLiteral("%TEXTPIPE=\"\"")
                                         << QStringLiteral("%OHEADER=\"\"")
                                         << QStringLiteral("%HEADER=\"\"")
                                         << QStringLiteral("%DICTIONARYLANGUAGE=\"\"")
                                         << QStringLiteral("%LANGUAGE=\"\"");
    return keywordsWithArgs;
}

QStringList TemplateParser::Util::keywords()
{
    const QStringList keywords = QStringList()
                                 << QStringLiteral("%QUOTE")
                                 << QStringLiteral("%FORCEDPLAIN")
                                 << QStringLiteral("%FORCEDHTML")
                                 << QStringLiteral("%QHEADERS")
                                 << QStringLiteral("%HEADERS")
                                 << QStringLiteral("%TEXT")
                                 << QStringLiteral("%OTEXTSIZE")
                                 << QStringLiteral("%OTEXT")
                                 << QStringLiteral("%OADDRESSEESADDR")
                                 << QStringLiteral("%CCADDR")
                                 << QStringLiteral("%CCNAME")
                                 << QStringLiteral("%CCFNAME")
                                 << QStringLiteral("%CCLNAME")
                                 << QStringLiteral("%TOADDR")
                                 << QStringLiteral("%TONAME")
                                 << QStringLiteral("%TOFNAME")
                                 << QStringLiteral("%TOLNAME")
                                 << QStringLiteral("%TOLIST")
                                 << QStringLiteral("%FROMADDR")
                                 << QStringLiteral("%FROMNAME")
                                 << QStringLiteral("%FROMFNAME")
                                 << QStringLiteral("%FROMLNAME")
                                 << QStringLiteral("%FULLSUBJECT")
                                 << QStringLiteral("%MSGID")
                                 << QStringLiteral("%HEADER\\( ")
                                 << QStringLiteral("%OCCADDR")
                                 << QStringLiteral("%OCCNAME")
                                 << QStringLiteral("%OCCFNAME")
                                 << QStringLiteral("%OCCLNAME")
                                 << QStringLiteral("%OTOADDR")
                                 << QStringLiteral("%OTONAME")
                                 << QStringLiteral("%OTOFNAME")
                                 << QStringLiteral("%OTOLNAME")
                                 << QStringLiteral("%OTOLIST")
                                 << QStringLiteral("%OTO")
                                 << QStringLiteral("%OFROMADDR")
                                 << QStringLiteral("%OFROMNAME")
                                 << QStringLiteral("%OFROMFNAME")
                                 << QStringLiteral("%OFROMLNAME")
                                 << QStringLiteral("%OFULLSUBJECT")
                                 << QStringLiteral("%OFULLSUBJ")
                                 << QStringLiteral("%OMSGID")
                                 << QStringLiteral("%DATEEN")
                                 << QStringLiteral("%DATESHORT")
                                 << QStringLiteral("%DATE")
                                 << QStringLiteral("%DOW")
                                 << QStringLiteral("%TIMELONGEN")
                                 << QStringLiteral("%TIMELONG")
                                 << QStringLiteral("%TIME")
                                 << QStringLiteral("%ODATEEN")
                                 << QStringLiteral("%ODATESHORT")
                                 << QStringLiteral("%ODATE")
                                 << QStringLiteral("%ODOW")
                                 << QStringLiteral("%OTIMELONGEN")
                                 << QStringLiteral("%OTIMELONG")
                                 << QStringLiteral("%OTIME")
                                 << QStringLiteral("%BLANK")
                                 << QStringLiteral("%NOP")
                                 << QStringLiteral("%CLEAR")
                                 << QStringLiteral("%DEBUGOFF")
                                 << QStringLiteral("%DEBUG")
                                 << QStringLiteral("%CURSOR")
                                 << QStringLiteral("%SIGNATURE");
    return keywords;
}

QString TemplateParser::Util::getFirstNameFromEmail(const QString &str)
{
    // simple logic:
    // if there is ',' in name, than format is 'Last, First'
    // else format is 'First Last'
    // last resort -- return 'name' from 'name@domain'
    int sep_pos;
    QString res;
    if ((sep_pos = str.indexOf(QLatin1Char('@'))) > 0) {
        int i;
        for (i = (sep_pos - 1); i >= 0; --i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.prepend(c);
            } else {
                break;
            }
        }
    } else if ((sep_pos = str.indexOf(QLatin1Char(','))) > 0) {
        int i;
        bool begin = false;
        const int strLength(str.length());
        for (i = sep_pos; i < strLength; ++i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                begin = true;
                res.append(c);
            } else if (begin) {
                break;
            }
        }
    } else {
        int i;
        const int strLength(str.length());
        for (i = 0; i < strLength; ++i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.append(c);
            } else {
                break;
            }
        }
    }
    return res;
}

QString TemplateParser::Util::getLastNameFromEmail(const QString &str)
{
    // simple logic:
    // if there is ',' in name, than format is 'Last, First'
    // else format is 'First Last'
    int sep_pos;
    QString res;
    if ((sep_pos = str.indexOf(QLatin1Char(','))) > 0) {
        int i;
        for (i = sep_pos; i >= 0; --i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.prepend(c);
            } else {
                break;
            }
        }
    } else {
        if ((sep_pos = str.indexOf(QLatin1Char(' '))) > 0) {
            bool begin = false;
            const int strLength(str.length());
            for (int i = sep_pos; i < strLength; ++i) {
                QChar c = str[i];
                if (c.isLetterOrNumber()) {
                    begin = true;
                    res.append(c);
                } else if (begin) {
                    break;
                }
            }
        }
    }
    return res;
}

QString TemplateParser::Util::removeSpaceAtBegin(const QString &selection)
{
    QString content = selection;
    // Remove blank lines at the beginning:
    const int firstNonWS = content.indexOf(QRegularExpression(QLatin1String("\\S")));
    const int lineStart = content.lastIndexOf(QLatin1Char('\n'), firstNonWS);
    if (lineStart >= 0) {
        content.remove(0, lineStart);
    }
    return content;
}
