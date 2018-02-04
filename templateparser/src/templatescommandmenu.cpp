/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

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


#include "templatescommandmenu.h"

#include <QAction>
#include <KActionMenu>
#include "templateparser_debug.h"
#include <KLocalizedString>
#include <QAction>

#undef I18N_NOOP
#define I18N_NOOP(t) nullptr, t
#undef I18N_NOOP2
#define I18N_NOOP2(c, t) c, t

using namespace TemplateParser;

struct InsertCommand {
    const char *context;
    const char *name;
    const TemplatesCommandMenu::Command command;

    QString getLocalizedDisplayName() const
    {
        return i18nc(context, name);
    }
};

static const InsertCommand originalCommands[] = {
    {
        I18N_NOOP("Quoted Message Text"),
        TemplatesCommandMenu::CQuote
    },

    {
        I18N_NOOP("Message Text as Is"),
        TemplatesCommandMenu::CText
    },

    {
        I18N_NOOP("Message Id"),
        TemplatesCommandMenu::COMsgId
    },

    {
        I18N_NOOP("Date"),
        TemplatesCommandMenu::CODate
    },

    {
        I18N_NOOP("Date in Short Format"),
        TemplatesCommandMenu::CODateShort
    },

    {
        I18N_NOOP("Date in C Locale"),
        TemplatesCommandMenu::CODateEn
    },

    {
        I18N_NOOP("Day of Week"),
        TemplatesCommandMenu::CODow
    },

    {
        I18N_NOOP("Time"),
        TemplatesCommandMenu::COTime
    },

    {
        I18N_NOOP("Time in Long Format"),
        TemplatesCommandMenu::COTimeLong
    },

    {
        I18N_NOOP("Time in C Locale"),
        TemplatesCommandMenu::COTimeLongEn
    },

    {
        I18N_NOOP("To Field Address"),
        TemplatesCommandMenu::COToAddr
    },

    {
        I18N_NOOP("To Field Name"),
        TemplatesCommandMenu::COToName
    },

    {
        I18N_NOOP("To Field First Name"),
        TemplatesCommandMenu::COToFName
    },

    {
        I18N_NOOP("To Field Last Name"),
        TemplatesCommandMenu::COToLName
    },

    {
        I18N_NOOP("CC Field Address"),
        TemplatesCommandMenu::COCCAddr
    },

    {
        I18N_NOOP("CC Field Name"),
        TemplatesCommandMenu::COCCName
    },

    {
        I18N_NOOP("CC Field First Name"),
        TemplatesCommandMenu::COCCFName
    },

    {
        I18N_NOOP("CC Field Last Name"),
        TemplatesCommandMenu::COCCLName
    },

    {
        I18N_NOOP("From Field Address"),
        TemplatesCommandMenu::COFromAddr
    },

    {
        I18N_NOOP("From Field Name"),
        TemplatesCommandMenu::COFromName
    },

    {
        I18N_NOOP("From Field First Name"),
        TemplatesCommandMenu::COFromFName
    },

    {
        I18N_NOOP("From Field Last Name"),
        TemplatesCommandMenu::COFromLName
    },

    {
        I18N_NOOP("Addresses of all recipients"),
        TemplatesCommandMenu::COAddresseesAddr
    },

    {
        I18N_NOOP2("Template value for subject of the message", "Subject"),
        TemplatesCommandMenu::COFullSubject
    },

    {
        I18N_NOOP("Quoted Headers"),
        TemplatesCommandMenu::CQHeaders
    },

    {
        I18N_NOOP("Headers as Is"),
        TemplatesCommandMenu::CHeaders
    },

    {
        I18N_NOOP("Header Content"),
        TemplatesCommandMenu::COHeader
    },

    {
        I18N_NOOP("Reply as Quoted Plain Text"),
        TemplatesCommandMenu::CQuotePlain
    },

    {
        I18N_NOOP("Reply as Quoted HTML Text"),
        TemplatesCommandMenu::CQuoteHtml
    }
};
static const int originalCommandsCount
    = sizeof(originalCommands) / sizeof(*originalCommands);

static const InsertCommand currentCommands[] = {
    {
        I18N_NOOP("Date"),
        TemplatesCommandMenu::CDate
    },

    {
        I18N_NOOP("Date in Short Format"),
        TemplatesCommandMenu::CDateShort
    },

    {
        I18N_NOOP("Date in C Locale"),
        TemplatesCommandMenu::CDateEn
    },

    {
        I18N_NOOP("Day of Week"),
        TemplatesCommandMenu::CDow
    },

    {
        I18N_NOOP("Time"),
        TemplatesCommandMenu::CTime
    },

    {
        I18N_NOOP("Time in Long Format"),
        TemplatesCommandMenu::CTimeLong
    },

    {
        I18N_NOOP("Time in C Locale"),
        TemplatesCommandMenu::CTimeLongEn
    },
    {
        I18N_NOOP("To Field Address"),
        TemplatesCommandMenu::CToAddr
    },

    {
        I18N_NOOP("To Field Name"),
        TemplatesCommandMenu::CToName
    },

    {
        I18N_NOOP("To Field First Name"),
        TemplatesCommandMenu::CToFName
    },

    {
        I18N_NOOP("To Field Last Name"),
        TemplatesCommandMenu::CToLName
    },

    {
        I18N_NOOP("CC Field Address"),
        TemplatesCommandMenu::CCCAddr
    },

    {
        I18N_NOOP("CC Field Name"),
        TemplatesCommandMenu::CCCName
    },

    {
        I18N_NOOP("CC Field First Name"),
        TemplatesCommandMenu::CCCFName
    },

    {
        I18N_NOOP("CC Field Last Name"),
        TemplatesCommandMenu::CCCLName
    },

    {
        I18N_NOOP("From Field Address"),
        TemplatesCommandMenu::CFromAddr
    },

    {
        I18N_NOOP("From field Name"),
        TemplatesCommandMenu::CFromName
    },

    {
        I18N_NOOP("From Field First Name"),
        TemplatesCommandMenu::CFromFName
    },

    {
        I18N_NOOP("From Field Last Name"),
        TemplatesCommandMenu::CFromLName
    },

    {
        I18N_NOOP2("Template subject command.", "Subject"),
        TemplatesCommandMenu::CFullSubject
    },

    {
        I18N_NOOP("Header Content"),
        TemplatesCommandMenu::CHeader
    }
};
static const int currentCommandsCount = sizeof(currentCommands) / sizeof(*currentCommands);

static const InsertCommand extCommands[] = {
    {
        I18N_NOOP("Pipe Original Message Body and Insert Result as Quoted Text"),
        TemplatesCommandMenu::CQuotePipe
    },

    {
        I18N_NOOP("Pipe Original Message Body and Insert Result as Is"),
        TemplatesCommandMenu::CTextPipe
    },

    {
        I18N_NOOP("Pipe Original Message with Headers and Insert Result as Is"),
        TemplatesCommandMenu::CMsgPipe
    },

    {
        I18N_NOOP("Pipe Current Message Body and Insert Result as Is"),
        TemplatesCommandMenu::CBodyPipe
    },

    {
        I18N_NOOP("Pipe Current Message Body and Replace with Result"),
        TemplatesCommandMenu::CClearPipe
    }
};

static const int extCommandsCount
    = sizeof(extCommands) / sizeof(*extCommands);

static const InsertCommand miscCommands[] = {
    {
        I18N_NOOP2("Inserts user signature, also known as footer, into message", "Signature"),
        TemplatesCommandMenu::CSignature
    },

    {
        I18N_NOOP("Insert File Content"),
        TemplatesCommandMenu::CInsert
    },

    {
        I18N_NOOP2("All characters, up to and including the next newline, "
                   "are discarded without performing any macro expansion",
                   "Discard to Next Line"),
        TemplatesCommandMenu::CDnl
    },

    {
        I18N_NOOP("Template Comment"),
        TemplatesCommandMenu::CRem
    },

    {
        I18N_NOOP("No Operation"),
        TemplatesCommandMenu::CNop
    },

    {
        I18N_NOOP("Clear Generated Message"),
        TemplatesCommandMenu::CClear
    },

    {
        I18N_NOOP("Turn Debug On"),
        TemplatesCommandMenu::CDebug
    },

    {
        I18N_NOOP("Turn Debug Off"),
        TemplatesCommandMenu::CDebugOff
    },

    {
        I18N_NOOP("Cursor position"),
        TemplatesCommandMenu::CCursor
    },

    {
        I18N_NOOP("Blank text"),
        TemplatesCommandMenu::CBlank
    },

    {
        I18N_NOOP("Dictionary Language"),
        TemplatesCommandMenu::CDictionaryLanguage
    },
    {
        I18N_NOOP("Language"),
        TemplatesCommandMenu::CLanguage
    },
    //TODO add support for custom variable. %CUSTOM="???" ?
};
static const int miscCommandsCount = sizeof(miscCommands) / sizeof(*miscCommands);

void TemplatesCommandMenu::fillMenuFromActionMap(const QMap< QString, TemplatesCommandMenu::Command > &map, KActionMenu *menu)
{
    QMap< QString, TemplatesCommandMenu::Command >::const_iterator it = map.constBegin();
    QMap< QString, TemplatesCommandMenu::Command >::const_iterator end = map.constEnd();

    while (it != end) {
        QAction *action = new QAction(it.key(), menu);   //krazy:exclude=tipsandthis
        const TemplatesCommandMenu::Command cmd = it.value();
        connect(action, &QAction::triggered, this, [this, cmd]{ slotInsertCommand(cmd); });
        menu->addAction(action);
        ++it;
    }
}

TemplatesCommandMenu::TemplatesCommandMenu(QObject *parent)
    : QObject(parent)
{
    fillMenu();
}

TemplatesCommandMenu::~TemplatesCommandMenu()
{

}

QMenu *TemplatesCommandMenu::menu() const
{
    return mMenu->menu();
}

void TemplatesCommandMenu::fillMenu()
{
    QMap< QString, Command > commandMap;


    mMenu = new KActionMenu(i18n("Insert Command"), this);

    // ******************************************************
    KActionMenu *menu = new KActionMenu(i18n("Original Message"), mMenu);
    mMenu->addAction(menu);

    // Map sorts commands
    for (int i = 0; i < originalCommandsCount; ++i) {
        commandMap.insert(originalCommands[i].getLocalizedDisplayName(), originalCommands[i].command);
    }

    fillMenuFromActionMap(commandMap, menu);
    commandMap.clear();

    // ******************************************************
    menu = new KActionMenu(i18n("Current Message"), mMenu);
    mMenu->addAction(menu);

    for (int i = 0; i < currentCommandsCount; ++i) {
        commandMap.insert(currentCommands[i].getLocalizedDisplayName(), currentCommands[i].command);
    }

    fillMenuFromActionMap(commandMap, menu);
    commandMap.clear();

    // ******************************************************
    menu = new KActionMenu(i18n("Process with External Programs"), mMenu);
    mMenu->addAction(menu);

    for (int i = 0; i < extCommandsCount; ++i) {
        commandMap.insert(extCommands[i].getLocalizedDisplayName(), extCommands[i].command);
    }

    fillMenuFromActionMap(commandMap, menu);
    commandMap.clear();

    // ******************************************************
    menu = new KActionMenu(i18nc("Miscellaneous template commands menu", "Miscellaneous"), mMenu);
    mMenu->addAction(menu);

    for (int i = 0; i < miscCommandsCount; ++i) {
        commandMap.insert(miscCommands[i].getLocalizedDisplayName(), miscCommands[i].command);
    }

    fillMenuFromActionMap(commandMap, menu);

}


void TemplatesCommandMenu::slotInsertCommand(TemplatesCommandMenu::Command cmd)
{
    Q_EMIT insertCommand(cmd);

    switch (cmd) {
    case TemplatesCommandMenu::CBlank:
        Q_EMIT insertCommand(QStringLiteral("%BLANK"));
        break;
    case TemplatesCommandMenu::CQuote:
        Q_EMIT insertCommand(QStringLiteral("%QUOTE"));
        break;
    case TemplatesCommandMenu::CText:
        Q_EMIT insertCommand(QStringLiteral("%TEXT"));
        break;
    case TemplatesCommandMenu::COMsgId:
        Q_EMIT insertCommand(QStringLiteral("%OMSGID"));
        break;
    case TemplatesCommandMenu::CODate:
        Q_EMIT insertCommand(QStringLiteral("%ODATE"));
        break;
    case TemplatesCommandMenu::CODateShort:
        Q_EMIT insertCommand(QStringLiteral("%ODATESHORT"));
        break;
    case TemplatesCommandMenu::CODateEn:
        Q_EMIT insertCommand(QStringLiteral("%ODATEEN"));
        break;
    case TemplatesCommandMenu::CODow:
        Q_EMIT insertCommand(QStringLiteral("%ODOW"));
        break;
    case TemplatesCommandMenu::COTime:
        Q_EMIT insertCommand(QStringLiteral("%OTIME"));
        break;
    case TemplatesCommandMenu::COTimeLong:
        Q_EMIT insertCommand(QStringLiteral("%OTIMELONG"));
        break;
    case TemplatesCommandMenu::COTimeLongEn:
        Q_EMIT insertCommand(QStringLiteral("%OTIMELONGEN"));
        break;
    case TemplatesCommandMenu::COToAddr:
        Q_EMIT insertCommand(QStringLiteral("%OTOADDR"));
        break;
    case TemplatesCommandMenu::COToName:
        Q_EMIT insertCommand(QStringLiteral("%OTONAME"));
        break;
    case TemplatesCommandMenu::COToFName:
        Q_EMIT insertCommand(QStringLiteral("%OTOFNAME"));
        break;
    case TemplatesCommandMenu::COToLName:
        Q_EMIT insertCommand(QStringLiteral("%OTOLNAME"));
        break;
    case TemplatesCommandMenu::COCCAddr:
        Q_EMIT insertCommand(QStringLiteral("%OCCADDR"));
        break;
    case TemplatesCommandMenu::COCCName:
        Q_EMIT insertCommand(QStringLiteral("%OCCNAME"));
        break;
    case TemplatesCommandMenu::COCCFName:
        Q_EMIT insertCommand(QStringLiteral("%OCCFNAME"));
        break;
    case TemplatesCommandMenu::COCCLName:
        Q_EMIT insertCommand(QStringLiteral("%OCCLNAME"));
        break;
    case TemplatesCommandMenu::COFromAddr:
        Q_EMIT insertCommand(QStringLiteral("%OFROMADDR"));
        break;
    case TemplatesCommandMenu::COFromName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMNAME"));
        break;
    case TemplatesCommandMenu::COFromFName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMFNAME"));
        break;
    case TemplatesCommandMenu::COFromLName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMLNAME"));
        break;
    case TemplatesCommandMenu::COFullSubject:
        Q_EMIT insertCommand(QStringLiteral("%OFULLSUBJECT"));
        break;
    case TemplatesCommandMenu::CQHeaders:
        Q_EMIT insertCommand(QStringLiteral("%QHEADERS"));
        break;
    case TemplatesCommandMenu::CHeaders:
        Q_EMIT insertCommand(QStringLiteral("%HEADERS"));
        break;
    case TemplatesCommandMenu::COHeader:
        Q_EMIT insertCommand(QStringLiteral("%OHEADER=\"\""), -1);
        break;
    case TemplatesCommandMenu::CMsgId:
        Q_EMIT insertCommand(QStringLiteral("%MSGID"));
        break;
    case TemplatesCommandMenu::CDate:
        Q_EMIT insertCommand(QStringLiteral("%DATE"));
        break;
    case TemplatesCommandMenu::CDateShort:
        Q_EMIT insertCommand(QStringLiteral("%DATESHORT"));
        break;
    case TemplatesCommandMenu::CDateEn:
        Q_EMIT insertCommand(QStringLiteral("%DATEEN"));
        break;
    case TemplatesCommandMenu::CDow:
        Q_EMIT insertCommand(QStringLiteral("%DOW"));
        break;
    case TemplatesCommandMenu::CTime:
        Q_EMIT insertCommand(QStringLiteral("%TIME"));
        break;
    case TemplatesCommandMenu::CTimeLong:
        Q_EMIT insertCommand(QStringLiteral("%TIMELONG"));
        break;
    case TemplatesCommandMenu::CTimeLongEn:
        Q_EMIT insertCommand(QStringLiteral("%TIMELONGEN"));
        break;
    case TemplatesCommandMenu::COAddresseesAddr:
        Q_EMIT insertCommand(QStringLiteral("%OADDRESSEESADDR"));
        break;
    case TemplatesCommandMenu::CToAddr:
        Q_EMIT insertCommand(QStringLiteral("%TOADDR"));
        break;
    case TemplatesCommandMenu::CToName:
        Q_EMIT insertCommand(QStringLiteral("%TONAME"));
        break;
    case TemplatesCommandMenu::CToFName:
        Q_EMIT insertCommand(QStringLiteral("%TOFNAME"));
        break;
    case TemplatesCommandMenu::CToLName:
        Q_EMIT insertCommand(QStringLiteral("%TOLNAME"));
        break;
    case TemplatesCommandMenu::CCCAddr:
        Q_EMIT insertCommand(QStringLiteral("%CCADDR"));
        break;
    case TemplatesCommandMenu::CCCName:
        Q_EMIT insertCommand(QStringLiteral("%CCNAME"));
        break;
    case TemplatesCommandMenu::CCCFName:
        Q_EMIT insertCommand(QStringLiteral("%CCFNAME"));
        break;
    case TemplatesCommandMenu::CCCLName:
        Q_EMIT insertCommand(QStringLiteral("%CCLNAME"));
        break;
    case TemplatesCommandMenu::CFromAddr:
        Q_EMIT insertCommand(QStringLiteral("%FROMADDR"));
        break;
    case TemplatesCommandMenu::CFromName:
        Q_EMIT insertCommand(QStringLiteral("%FROMNAME"));
        break;
    case TemplatesCommandMenu::CFromFName:
        Q_EMIT insertCommand(QStringLiteral("%FROMFNAME"));
        break;
    case TemplatesCommandMenu::CFromLName:
        Q_EMIT insertCommand(QStringLiteral("%FROMLNAME"));
        break;
    case TemplatesCommandMenu::CFullSubject:
        Q_EMIT insertCommand(QStringLiteral("%FULLSUBJECT"));
        break;
    case TemplatesCommandMenu::CHeader:
        Q_EMIT insertCommand(QStringLiteral("%HEADER=\"\""), -1);
        break;
    case TemplatesCommandMenu::CSystem:
        Q_EMIT insertCommand(QStringLiteral("%SYSTEM=\"\""), -1);
        break;
    case TemplatesCommandMenu::CQuotePipe:
        Q_EMIT insertCommand(QStringLiteral("%QUOTEPIPE=\"\""), -1);
        break;
    case TemplatesCommandMenu::CTextPipe:
        Q_EMIT insertCommand(QStringLiteral("%TEXTPIPE=\"\""), -1);
        break;
    case TemplatesCommandMenu::CMsgPipe:
        Q_EMIT insertCommand(QStringLiteral("%MSGPIPE=\"\""), -1);
        break;
    case TemplatesCommandMenu::CBodyPipe:
        Q_EMIT insertCommand(QStringLiteral("%BODYPIPE=\"\""), -1);
        break;
    case TemplatesCommandMenu::CClearPipe:
        Q_EMIT insertCommand(QStringLiteral("%CLEARPIPE=\"\""), -1);
        break;
    case TemplatesCommandMenu::CCursor:
        Q_EMIT insertCommand(QStringLiteral("%CURSOR"));
        break;
    case TemplatesCommandMenu::CSignature:
        Q_EMIT insertCommand(QStringLiteral("%SIGNATURE"));
        break;
    case TemplatesCommandMenu::CInsert:
        Q_EMIT insertCommand(QStringLiteral("%INSERT=\"\""), -1);
        break;
    case TemplatesCommandMenu::CDnl:
        Q_EMIT insertCommand(QStringLiteral("%-"));
        break;
    case TemplatesCommandMenu::CRem:
        Q_EMIT insertCommand(QStringLiteral("%REM=\"\""), -1);
        break;
    case TemplatesCommandMenu::CNop:
        Q_EMIT insertCommand(QStringLiteral("%NOP"));
        break;
    case TemplatesCommandMenu::CClear:
        Q_EMIT insertCommand(QStringLiteral("%CLEAR"));
        break;
    case TemplatesCommandMenu::CDebug:
        Q_EMIT insertCommand(QStringLiteral("%DEBUG"));
        break;
    case TemplatesCommandMenu::CDebugOff:
        Q_EMIT insertCommand(QStringLiteral("%DEBUGOFF"));
        break;
    case TemplatesCommandMenu::CQuotePlain:
        Q_EMIT insertCommand(QStringLiteral("%FORCEDPLAIN"));
        break;
    case TemplatesCommandMenu::CQuoteHtml:
        Q_EMIT insertCommand(QStringLiteral("%FORCEDHTML"));
        break;
    case TemplatesCommandMenu::CDictionaryLanguage:
        Q_EMIT insertCommand(QStringLiteral("%DICTIONARYLANGUAGE=\"\""), -1);
        break;
    case TemplatesCommandMenu::CLanguage:
        Q_EMIT insertCommand(QStringLiteral("%LANGUAGE=\"\""), -1);
        break;
    default:
        qCDebug(TEMPLATEPARSER_LOG) << "Unknown template command index:" << cmd;
        break;
    }
}
