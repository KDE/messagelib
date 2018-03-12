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


static const InsertCommand debugCommands[] = {
    {
        I18N_NOOP("Turn Debug On"),
        TemplatesCommandMenu::CDebug
    },

    {
        I18N_NOOP("Turn Debug Off"),
        TemplatesCommandMenu::CDebugOff
    }
};
static const int debugCommandsCount = sizeof(debugCommands) / sizeof(*debugCommands);

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
    mMenu = new KActionMenu(i18n("Insert Command"), this);
}

void TemplatesCommandMenu::fillSubMenus()
{
    if (mWasInitialized) {
        return;
    }
    mWasInitialized = true;
    QMap< QString, Command > commandMap;
    KActionMenu *menu = nullptr;
    // ******************************************************
    if ((mType == All) | (mType == ReplyForwardMessage)) {
        menu = new KActionMenu(i18n("Original Message"), mMenu);
        mMenu->addAction(menu);

        // Map sorts commands
        for (int i = 0; i < originalCommandsCount; ++i) {
            commandMap.insert(originalCommands[i].getLocalizedDisplayName(), originalCommands[i].command);
        }

        fillMenuFromActionMap(commandMap, menu);
        commandMap.clear();
    }

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
    commandMap.clear();

    if (!qEnvironmentVariableIsEmpty("KDEPIM_DEBUGGING")) {
        // ******************************************************
        menu = new KActionMenu(i18nc("Debug template commands menu", "Debug"), mMenu);
        mMenu->addAction(menu);

        for (int i = 0; i < debugCommandsCount; ++i) {
            commandMap.insert(debugCommands[i].getLocalizedDisplayName(), debugCommands[i].command);
        }

        fillMenuFromActionMap(commandMap, menu);
    }

}

TemplatesCommandMenu::MenuType TemplatesCommandMenu::type() const
{
    return mType;
}

void TemplatesCommandMenu::setType(const MenuType &type)
{
    mType = type;
}


void TemplatesCommandMenu::slotInsertCommand(TemplatesCommandMenu::Command cmd)
{
    Q_EMIT insertCommand(cmd);

    switch (cmd) {
    case TemplatesCommandMenu::CBlank:
        Q_EMIT insertCommand(QStringLiteral("%BLANK"));
        return;
    case TemplatesCommandMenu::CQuote:
        Q_EMIT insertCommand(QStringLiteral("%QUOTE"));
        return;
    case TemplatesCommandMenu::CText:
        Q_EMIT insertCommand(QStringLiteral("%TEXT"));
        return;
    case TemplatesCommandMenu::COMsgId:
        Q_EMIT insertCommand(QStringLiteral("%OMSGID"));
        return;
    case TemplatesCommandMenu::CODate:
        Q_EMIT insertCommand(QStringLiteral("%ODATE"));
        return;
    case TemplatesCommandMenu::CODateShort:
        Q_EMIT insertCommand(QStringLiteral("%ODATESHORT"));
        return;
    case TemplatesCommandMenu::CODateEn:
        Q_EMIT insertCommand(QStringLiteral("%ODATEEN"));
        return;
    case TemplatesCommandMenu::CODow:
        Q_EMIT insertCommand(QStringLiteral("%ODOW"));
        return;
    case TemplatesCommandMenu::COTime:
        Q_EMIT insertCommand(QStringLiteral("%OTIME"));
        return;
    case TemplatesCommandMenu::COTimeLong:
        Q_EMIT insertCommand(QStringLiteral("%OTIMELONG"));
        return;
    case TemplatesCommandMenu::COTimeLongEn:
        Q_EMIT insertCommand(QStringLiteral("%OTIMELONGEN"));
        return;
    case TemplatesCommandMenu::COToAddr:
        Q_EMIT insertCommand(QStringLiteral("%OTOADDR"));
        return;
    case TemplatesCommandMenu::COToName:
        Q_EMIT insertCommand(QStringLiteral("%OTONAME"));
        return;
    case TemplatesCommandMenu::COToFName:
        Q_EMIT insertCommand(QStringLiteral("%OTOFNAME"));
        return;
    case TemplatesCommandMenu::COToLName:
        Q_EMIT insertCommand(QStringLiteral("%OTOLNAME"));
        return;
    case TemplatesCommandMenu::COCCAddr:
        Q_EMIT insertCommand(QStringLiteral("%OCCADDR"));
        return;
    case TemplatesCommandMenu::COCCName:
        Q_EMIT insertCommand(QStringLiteral("%OCCNAME"));
        return;
    case TemplatesCommandMenu::COCCFName:
        Q_EMIT insertCommand(QStringLiteral("%OCCFNAME"));
        return;
    case TemplatesCommandMenu::COCCLName:
        Q_EMIT insertCommand(QStringLiteral("%OCCLNAME"));
        return;
    case TemplatesCommandMenu::COFromAddr:
        Q_EMIT insertCommand(QStringLiteral("%OFROMADDR"));
        return;
    case TemplatesCommandMenu::COFromName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMNAME"));
        return;
    case TemplatesCommandMenu::COFromFName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMFNAME"));
        return;
    case TemplatesCommandMenu::COFromLName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMLNAME"));
        return;
    case TemplatesCommandMenu::COFullSubject:
        Q_EMIT insertCommand(QStringLiteral("%OFULLSUBJECT"));
        return;
    case TemplatesCommandMenu::CQHeaders:
        Q_EMIT insertCommand(QStringLiteral("%QHEADERS"));
        return;
    case TemplatesCommandMenu::CHeaders:
        Q_EMIT insertCommand(QStringLiteral("%HEADERS"));
        return;
    case TemplatesCommandMenu::COHeader:
        Q_EMIT insertCommand(QStringLiteral("%OHEADER=\"\""), -1);
        return;
    case TemplatesCommandMenu::CMsgId:
        Q_EMIT insertCommand(QStringLiteral("%MSGID"));
        return;
    case TemplatesCommandMenu::CDate:
        Q_EMIT insertCommand(QStringLiteral("%DATE"));
        return;
    case TemplatesCommandMenu::CDateShort:
        Q_EMIT insertCommand(QStringLiteral("%DATESHORT"));
        return;
    case TemplatesCommandMenu::CDateEn:
        Q_EMIT insertCommand(QStringLiteral("%DATEEN"));
        return;
    case TemplatesCommandMenu::CDow:
        Q_EMIT insertCommand(QStringLiteral("%DOW"));
        return;
    case TemplatesCommandMenu::CTime:
        Q_EMIT insertCommand(QStringLiteral("%TIME"));
        return;
    case TemplatesCommandMenu::CTimeLong:
        Q_EMIT insertCommand(QStringLiteral("%TIMELONG"));
        return;
    case TemplatesCommandMenu::CTimeLongEn:
        Q_EMIT insertCommand(QStringLiteral("%TIMELONGEN"));
        return;
    case TemplatesCommandMenu::COAddresseesAddr:
        Q_EMIT insertCommand(QStringLiteral("%OADDRESSEESADDR"));
        return;
    case TemplatesCommandMenu::CToAddr:
        Q_EMIT insertCommand(QStringLiteral("%TOADDR"));
        return;
    case TemplatesCommandMenu::CToName:
        Q_EMIT insertCommand(QStringLiteral("%TONAME"));
        return;
    case TemplatesCommandMenu::CToFName:
        Q_EMIT insertCommand(QStringLiteral("%TOFNAME"));
        return;
    case TemplatesCommandMenu::CToLName:
        Q_EMIT insertCommand(QStringLiteral("%TOLNAME"));
        return;
    case TemplatesCommandMenu::CCCAddr:
        Q_EMIT insertCommand(QStringLiteral("%CCADDR"));
        return;
    case TemplatesCommandMenu::CCCName:
        Q_EMIT insertCommand(QStringLiteral("%CCNAME"));
        return;
    case TemplatesCommandMenu::CCCFName:
        Q_EMIT insertCommand(QStringLiteral("%CCFNAME"));
        return;
    case TemplatesCommandMenu::CCCLName:
        Q_EMIT insertCommand(QStringLiteral("%CCLNAME"));
        return;
    case TemplatesCommandMenu::CFromAddr:
        Q_EMIT insertCommand(QStringLiteral("%FROMADDR"));
        return;
    case TemplatesCommandMenu::CFromName:
        Q_EMIT insertCommand(QStringLiteral("%FROMNAME"));
        return;
    case TemplatesCommandMenu::CFromFName:
        Q_EMIT insertCommand(QStringLiteral("%FROMFNAME"));
        return;
    case TemplatesCommandMenu::CFromLName:
        Q_EMIT insertCommand(QStringLiteral("%FROMLNAME"));
        return;
    case TemplatesCommandMenu::CFullSubject:
        Q_EMIT insertCommand(QStringLiteral("%FULLSUBJECT"));
        return;
    case TemplatesCommandMenu::CHeader:
        Q_EMIT insertCommand(QStringLiteral("%HEADER=\"\""), -1);
        return;
    case TemplatesCommandMenu::CSystem:
        Q_EMIT insertCommand(QStringLiteral("%SYSTEM=\"\""), -1);
        return;
    case TemplatesCommandMenu::CQuotePipe:
        Q_EMIT insertCommand(QStringLiteral("%QUOTEPIPE=\"\""), -1);
        return;
    case TemplatesCommandMenu::CTextPipe:
        Q_EMIT insertCommand(QStringLiteral("%TEXTPIPE=\"\""), -1);
        return;
    case TemplatesCommandMenu::CMsgPipe:
        Q_EMIT insertCommand(QStringLiteral("%MSGPIPE=\"\""), -1);
        return;
    case TemplatesCommandMenu::CBodyPipe:
        Q_EMIT insertCommand(QStringLiteral("%BODYPIPE=\"\""), -1);
        return;
    case TemplatesCommandMenu::CClearPipe:
        Q_EMIT insertCommand(QStringLiteral("%CLEARPIPE=\"\""), -1);
        return;
    case TemplatesCommandMenu::CCursor:
        Q_EMIT insertCommand(QStringLiteral("%CURSOR"));
        return;
    case TemplatesCommandMenu::CSignature:
        Q_EMIT insertCommand(QStringLiteral("%SIGNATURE"));
        return;
    case TemplatesCommandMenu::CInsert:
        Q_EMIT insertCommand(QStringLiteral("%INSERT=\"\""), -1);
        return;
    case TemplatesCommandMenu::CDnl:
        Q_EMIT insertCommand(QStringLiteral("%-"));
        return;
    case TemplatesCommandMenu::CRem:
        Q_EMIT insertCommand(QStringLiteral("%REM=\"\""), -1);
        return;
    case TemplatesCommandMenu::CNop:
        Q_EMIT insertCommand(QStringLiteral("%NOP"));
        return;
    case TemplatesCommandMenu::CClear:
        Q_EMIT insertCommand(QStringLiteral("%CLEAR"));
        return;
    case TemplatesCommandMenu::CDebug:
        Q_EMIT insertCommand(QStringLiteral("%DEBUG"));
        return;
    case TemplatesCommandMenu::CDebugOff:
        Q_EMIT insertCommand(QStringLiteral("%DEBUGOFF"));
        return;
    case TemplatesCommandMenu::CQuotePlain:
        Q_EMIT insertCommand(QStringLiteral("%FORCEDPLAIN"));
        return;
    case TemplatesCommandMenu::CQuoteHtml:
        Q_EMIT insertCommand(QStringLiteral("%FORCEDHTML"));
        return;
    case TemplatesCommandMenu::CDictionaryLanguage:
        Q_EMIT insertCommand(QStringLiteral("%DICTIONARYLANGUAGE=\"\""), -1);
        return;
    case TemplatesCommandMenu::CLanguage:
        Q_EMIT insertCommand(QStringLiteral("%LANGUAGE=\"\""), -1);
        return;
    }
    qCDebug(TEMPLATEPARSER_LOG) << "Unknown template command index:" << cmd;
}
