/*
 * Copyright (C) 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "templatesinsertcommandpushbutton.h"

#include <QAction>
#include <KActionMenu>
#include "templateparser_debug.h"
#include <KLocalizedString>

#undef I18N_NOOP
#define I18N_NOOP(t) nullptr, t
#undef I18N_NOOP2
#define I18N_NOOP2(c, t) c, t

using namespace TemplateParser;

struct InsertCommand {
    const char *context;
    const char *name;
    const TemplatesInsertCommandPushButton::Command command;

    QString getLocalizedDisplayName() const
    {
        return i18nc(context, name);
    }
};

static const InsertCommand originalCommands[] = {
    {
        I18N_NOOP("Quoted Message Text"),
        TemplatesInsertCommandPushButton::CQuote
    },

    {
        I18N_NOOP("Message Text as Is"),
        TemplatesInsertCommandPushButton::CText
    },

    {
        I18N_NOOP("Message Id"),
        TemplatesInsertCommandPushButton::COMsgId
    },

    {
        I18N_NOOP("Date"),
        TemplatesInsertCommandPushButton::CODate
    },

    {
        I18N_NOOP("Date in Short Format"),
        TemplatesInsertCommandPushButton::CODateShort
    },

    {
        I18N_NOOP("Date in C Locale"),
        TemplatesInsertCommandPushButton::CODateEn
    },

    {
        I18N_NOOP("Day of Week"),
        TemplatesInsertCommandPushButton::CODow
    },

    {
        I18N_NOOP("Time"),
        TemplatesInsertCommandPushButton::COTime
    },

    {
        I18N_NOOP("Time in Long Format"),
        TemplatesInsertCommandPushButton::COTimeLong
    },

    {
        I18N_NOOP("Time in C Locale"),
        TemplatesInsertCommandPushButton::COTimeLongEn
    },

    {
        I18N_NOOP("To Field Address"),
        TemplatesInsertCommandPushButton::COToAddr
    },

    {
        I18N_NOOP("To Field Name"),
        TemplatesInsertCommandPushButton::COToName
    },

    {
        I18N_NOOP("To Field First Name"),
        TemplatesInsertCommandPushButton::COToFName
    },

    {
        I18N_NOOP("To Field Last Name"),
        TemplatesInsertCommandPushButton::COToLName
    },

    {
        I18N_NOOP("CC Field Address"),
        TemplatesInsertCommandPushButton::COCCAddr
    },

    {
        I18N_NOOP("CC Field Name"),
        TemplatesInsertCommandPushButton::COCCName
    },

    {
        I18N_NOOP("CC Field First Name"),
        TemplatesInsertCommandPushButton::COCCFName
    },

    {
        I18N_NOOP("CC Field Last Name"),
        TemplatesInsertCommandPushButton::COCCLName
    },

    {
        I18N_NOOP("From Field Address"),
        TemplatesInsertCommandPushButton::COFromAddr
    },

    {
        I18N_NOOP("From Field Name"),
        TemplatesInsertCommandPushButton::COFromName
    },

    {
        I18N_NOOP("From Field First Name"),
        TemplatesInsertCommandPushButton::COFromFName
    },

    {
        I18N_NOOP("From Field Last Name"),
        TemplatesInsertCommandPushButton::COFromLName
    },

    {
        I18N_NOOP("Addresses of all recipients"),
        TemplatesInsertCommandPushButton::COAddresseesAddr
    },

    {
        I18N_NOOP2("Template value for subject of the message", "Subject"),
        TemplatesInsertCommandPushButton::COFullSubject
    },

    {
        I18N_NOOP("Quoted Headers"),
        TemplatesInsertCommandPushButton::CQHeaders
    },

    {
        I18N_NOOP("Headers as Is"),
        TemplatesInsertCommandPushButton::CHeaders
    },

    {
        I18N_NOOP("Header Content"),
        TemplatesInsertCommandPushButton::COHeader
    },

    {
        I18N_NOOP("Reply as Quoted Plain Text"),
        TemplatesInsertCommandPushButton::CQuotePlain
    },

    {
        I18N_NOOP("Reply as Quoted HTML Text"),
        TemplatesInsertCommandPushButton::CQuoteHtml
    }
};
static const int originalCommandsCount
    = sizeof(originalCommands) / sizeof(*originalCommands);

static const InsertCommand currentCommands[] = {
    {
        I18N_NOOP("Date"),
        TemplatesInsertCommandPushButton::CDate
    },

    {
        I18N_NOOP("Date in Short Format"),
        TemplatesInsertCommandPushButton::CDateShort
    },

    {
        I18N_NOOP("Date in C Locale"),
        TemplatesInsertCommandPushButton::CDateEn
    },

    {
        I18N_NOOP("Day of Week"),
        TemplatesInsertCommandPushButton::CDow
    },

    {
        I18N_NOOP("Time"),
        TemplatesInsertCommandPushButton::CTime
    },

    {
        I18N_NOOP("Time in Long Format"),
        TemplatesInsertCommandPushButton::CTimeLong
    },

    {
        I18N_NOOP("Time in C Locale"),
        TemplatesInsertCommandPushButton::CTimeLongEn
    },
    {
        I18N_NOOP("To Field Address"),
        TemplatesInsertCommandPushButton::CToAddr
    },

    {
        I18N_NOOP("To Field Name"),
        TemplatesInsertCommandPushButton::CToName
    },

    {
        I18N_NOOP("To Field First Name"),
        TemplatesInsertCommandPushButton::CToFName
    },

    {
        I18N_NOOP("To Field Last Name"),
        TemplatesInsertCommandPushButton::CToLName
    },

    {
        I18N_NOOP("CC Field Address"),
        TemplatesInsertCommandPushButton::CCCAddr
    },

    {
        I18N_NOOP("CC Field Name"),
        TemplatesInsertCommandPushButton::CCCName
    },

    {
        I18N_NOOP("CC Field First Name"),
        TemplatesInsertCommandPushButton::CCCFName
    },

    {
        I18N_NOOP("CC Field Last Name"),
        TemplatesInsertCommandPushButton::CCCLName
    },

    {
        I18N_NOOP("From Field Address"),
        TemplatesInsertCommandPushButton::CFromAddr
    },

    {
        I18N_NOOP("From field Name"),
        TemplatesInsertCommandPushButton::CFromName
    },

    {
        I18N_NOOP("From Field First Name"),
        TemplatesInsertCommandPushButton::CFromFName
    },

    {
        I18N_NOOP("From Field Last Name"),
        TemplatesInsertCommandPushButton::CFromLName
    },

    {
        I18N_NOOP2("Template subject command.", "Subject"),
        TemplatesInsertCommandPushButton::CFullSubject
    },

    {
        I18N_NOOP("Header Content"),
        TemplatesInsertCommandPushButton::CHeader
    }
};
static const int currentCommandsCount = sizeof(currentCommands) / sizeof(*currentCommands);

static const InsertCommand extCommands[] = {
    {
        I18N_NOOP("Pipe Original Message Body and Insert Result as Quoted Text"),
        TemplatesInsertCommandPushButton::CQuotePipe
    },

    {
        I18N_NOOP("Pipe Original Message Body and Insert Result as Is"),
        TemplatesInsertCommandPushButton::CTextPipe
    },

    {
        I18N_NOOP("Pipe Original Message with Headers and Insert Result as Is"),
        TemplatesInsertCommandPushButton::CMsgPipe
    },

    {
        I18N_NOOP("Pipe Current Message Body and Insert Result as Is"),
        TemplatesInsertCommandPushButton::CBodyPipe
    },

    {
        I18N_NOOP("Pipe Current Message Body and Replace with Result"),
        TemplatesInsertCommandPushButton::CClearPipe
    }
};

static const int extCommandsCount
    = sizeof(extCommands) / sizeof(*extCommands);

static const InsertCommand miscCommands[] = {
    {
        I18N_NOOP2("Inserts user signature, also known as footer, into message", "Signature"),
        TemplatesInsertCommandPushButton::CSignature
    },

    {
        I18N_NOOP("Insert File Content"),
        TemplatesInsertCommandPushButton::CInsert
    },

    {
        I18N_NOOP2("All characters, up to and including the next newline, "
                   "are discarded without performing any macro expansion",
                   "Discard to Next Line"),
        TemplatesInsertCommandPushButton::CDnl
    },

    {
        I18N_NOOP("Template Comment"),
        TemplatesInsertCommandPushButton::CRem
    },

    {
        I18N_NOOP("No Operation"),
        TemplatesInsertCommandPushButton::CNop
    },

    {
        I18N_NOOP("Clear Generated Message"),
        TemplatesInsertCommandPushButton::CClear
    },

    {
        I18N_NOOP("Turn Debug On"),
        TemplatesInsertCommandPushButton::CDebug
    },

    {
        I18N_NOOP("Turn Debug Off"),
        TemplatesInsertCommandPushButton::CDebugOff
    },

    {
        I18N_NOOP("Cursor position"),
        TemplatesInsertCommandPushButton::CCursor
    },

    {
        I18N_NOOP("Blank text"),
        TemplatesInsertCommandPushButton::CBlank
    },

    {
        I18N_NOOP("Dictionary Language"),
        TemplatesInsertCommandPushButton::CDictionaryLanguage
    },
    {
        I18N_NOOP("Language"),
        TemplatesInsertCommandPushButton::CLanguage
    },
    //TODO add support for custom variable. %CUSTOM="???" ?
};
static const int miscCommandsCount = sizeof(miscCommands) / sizeof(*miscCommands);

void TemplatesInsertCommandPushButton::fillMenuFromActionMap(const QMap< QString, TemplatesInsertCommandPushButton::Command > &map, KActionMenu *menu)
{
    QMap< QString, TemplatesInsertCommandPushButton::Command >::const_iterator it = map.constBegin();
    QMap< QString, TemplatesInsertCommandPushButton::Command >::const_iterator end = map.constEnd();

    while (it != end) {
        QAction *action = new QAction(it.key(), menu);   //krazy:exclude=tipsandthis
        const TemplatesInsertCommandPushButton::Command cmd = it.value();
        connect(action, &QAction::triggered, this, [this, cmd]{ slotInsertCommand(cmd); });
        menu->addAction(action);
        ++it;
    }
}

TemplatesInsertCommandPushButton::TemplatesInsertCommandPushButton(QWidget *parent, const QString &name)
    : QPushButton(parent)
{
    setObjectName(name);
    setText(i18n("&Insert Command"));

    QMap< QString, Command > commandMap;


    mMenu = new KActionMenu(i18n("Insert Command"), this);
    setToolTip(
        i18nc("@info:tooltip",
              "Select a command to insert into the template"));
    setWhatsThis(
        i18nc("@info:whatsthis",
              "Traverse this menu to find a command to insert into the current template "
              "being edited.  The command will be inserted at the cursor location, "
              "so you want to move your cursor to the desired insertion point first."));

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

    setMenu(mMenu->menu());
}

TemplatesInsertCommandPushButton::~TemplatesInsertCommandPushButton()
{
}

void TemplatesInsertCommandPushButton::slotInsertCommand(TemplatesInsertCommandPushButton::Command cmd)
{
    Q_EMIT insertCommand(cmd);

    switch (cmd) {
    case TemplatesInsertCommandPushButton::CBlank:
        Q_EMIT insertCommand(QStringLiteral("%BLANK"));
        break;
    case TemplatesInsertCommandPushButton::CQuote:
        Q_EMIT insertCommand(QStringLiteral("%QUOTE"));
        break;
    case TemplatesInsertCommandPushButton::CText:
        Q_EMIT insertCommand(QStringLiteral("%TEXT"));
        break;
    case TemplatesInsertCommandPushButton::COMsgId:
        Q_EMIT insertCommand(QStringLiteral("%OMSGID"));
        break;
    case TemplatesInsertCommandPushButton::CODate:
        Q_EMIT insertCommand(QStringLiteral("%ODATE"));
        break;
    case TemplatesInsertCommandPushButton::CODateShort:
        Q_EMIT insertCommand(QStringLiteral("%ODATESHORT"));
        break;
    case TemplatesInsertCommandPushButton::CODateEn:
        Q_EMIT insertCommand(QStringLiteral("%ODATEEN"));
        break;
    case TemplatesInsertCommandPushButton::CODow:
        Q_EMIT insertCommand(QStringLiteral("%ODOW"));
        break;
    case TemplatesInsertCommandPushButton::COTime:
        Q_EMIT insertCommand(QStringLiteral("%OTIME"));
        break;
    case TemplatesInsertCommandPushButton::COTimeLong:
        Q_EMIT insertCommand(QStringLiteral("%OTIMELONG"));
        break;
    case TemplatesInsertCommandPushButton::COTimeLongEn:
        Q_EMIT insertCommand(QStringLiteral("%OTIMELONGEN"));
        break;
    case TemplatesInsertCommandPushButton::COToAddr:
        Q_EMIT insertCommand(QStringLiteral("%OTOADDR"));
        break;
    case TemplatesInsertCommandPushButton::COToName:
        Q_EMIT insertCommand(QStringLiteral("%OTONAME"));
        break;
    case TemplatesInsertCommandPushButton::COToFName:
        Q_EMIT insertCommand(QStringLiteral("%OTOFNAME"));
        break;
    case TemplatesInsertCommandPushButton::COToLName:
        Q_EMIT insertCommand(QStringLiteral("%OTOLNAME"));
        break;
    case TemplatesInsertCommandPushButton::COCCAddr:
        Q_EMIT insertCommand(QStringLiteral("%OCCADDR"));
        break;
    case TemplatesInsertCommandPushButton::COCCName:
        Q_EMIT insertCommand(QStringLiteral("%OCCNAME"));
        break;
    case TemplatesInsertCommandPushButton::COCCFName:
        Q_EMIT insertCommand(QStringLiteral("%OCCFNAME"));
        break;
    case TemplatesInsertCommandPushButton::COCCLName:
        Q_EMIT insertCommand(QStringLiteral("%OCCLNAME"));
        break;
    case TemplatesInsertCommandPushButton::COFromAddr:
        Q_EMIT insertCommand(QStringLiteral("%OFROMADDR"));
        break;
    case TemplatesInsertCommandPushButton::COFromName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMNAME"));
        break;
    case TemplatesInsertCommandPushButton::COFromFName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMFNAME"));
        break;
    case TemplatesInsertCommandPushButton::COFromLName:
        Q_EMIT insertCommand(QStringLiteral("%OFROMLNAME"));
        break;
    case TemplatesInsertCommandPushButton::COFullSubject:
        Q_EMIT insertCommand(QStringLiteral("%OFULLSUBJECT"));
        break;
    case TemplatesInsertCommandPushButton::CQHeaders:
        Q_EMIT insertCommand(QStringLiteral("%QHEADERS"));
        break;
    case TemplatesInsertCommandPushButton::CHeaders:
        Q_EMIT insertCommand(QStringLiteral("%HEADERS"));
        break;
    case TemplatesInsertCommandPushButton::COHeader:
        Q_EMIT insertCommand(QStringLiteral("%OHEADER=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CMsgId:
        Q_EMIT insertCommand(QStringLiteral("%MSGID"));
        break;
    case TemplatesInsertCommandPushButton::CDate:
        Q_EMIT insertCommand(QStringLiteral("%DATE"));
        break;
    case TemplatesInsertCommandPushButton::CDateShort:
        Q_EMIT insertCommand(QStringLiteral("%DATESHORT"));
        break;
    case TemplatesInsertCommandPushButton::CDateEn:
        Q_EMIT insertCommand(QStringLiteral("%DATEEN"));
        break;
    case TemplatesInsertCommandPushButton::CDow:
        Q_EMIT insertCommand(QStringLiteral("%DOW"));
        break;
    case TemplatesInsertCommandPushButton::CTime:
        Q_EMIT insertCommand(QStringLiteral("%TIME"));
        break;
    case TemplatesInsertCommandPushButton::CTimeLong:
        Q_EMIT insertCommand(QStringLiteral("%TIMELONG"));
        break;
    case TemplatesInsertCommandPushButton::CTimeLongEn:
        Q_EMIT insertCommand(QStringLiteral("%TIMELONGEN"));
        break;
    case TemplatesInsertCommandPushButton::COAddresseesAddr:
        Q_EMIT insertCommand(QStringLiteral("%OADDRESSEESADDR"));
        break;
    case TemplatesInsertCommandPushButton::CToAddr:
        Q_EMIT insertCommand(QStringLiteral("%TOADDR"));
        break;
    case TemplatesInsertCommandPushButton::CToName:
        Q_EMIT insertCommand(QStringLiteral("%TONAME"));
        break;
    case TemplatesInsertCommandPushButton::CToFName:
        Q_EMIT insertCommand(QStringLiteral("%TOFNAME"));
        break;
    case TemplatesInsertCommandPushButton::CToLName:
        Q_EMIT insertCommand(QStringLiteral("%TOLNAME"));
        break;
    case TemplatesInsertCommandPushButton::CCCAddr:
        Q_EMIT insertCommand(QStringLiteral("%CCADDR"));
        break;
    case TemplatesInsertCommandPushButton::CCCName:
        Q_EMIT insertCommand(QStringLiteral("%CCNAME"));
        break;
    case TemplatesInsertCommandPushButton::CCCFName:
        Q_EMIT insertCommand(QStringLiteral("%CCFNAME"));
        break;
    case TemplatesInsertCommandPushButton::CCCLName:
        Q_EMIT insertCommand(QStringLiteral("%CCLNAME"));
        break;
    case TemplatesInsertCommandPushButton::CFromAddr:
        Q_EMIT insertCommand(QStringLiteral("%FROMADDR"));
        break;
    case TemplatesInsertCommandPushButton::CFromName:
        Q_EMIT insertCommand(QStringLiteral("%FROMNAME"));
        break;
    case TemplatesInsertCommandPushButton::CFromFName:
        Q_EMIT insertCommand(QStringLiteral("%FROMFNAME"));
        break;
    case TemplatesInsertCommandPushButton::CFromLName:
        Q_EMIT insertCommand(QStringLiteral("%FROMLNAME"));
        break;
    case TemplatesInsertCommandPushButton::CFullSubject:
        Q_EMIT insertCommand(QStringLiteral("%FULLSUBJECT"));
        break;
    case TemplatesInsertCommandPushButton::CHeader:
        Q_EMIT insertCommand(QStringLiteral("%HEADER=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CSystem:
        Q_EMIT insertCommand(QStringLiteral("%SYSTEM=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CQuotePipe:
        Q_EMIT insertCommand(QStringLiteral("%QUOTEPIPE=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CTextPipe:
        Q_EMIT insertCommand(QStringLiteral("%TEXTPIPE=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CMsgPipe:
        Q_EMIT insertCommand(QStringLiteral("%MSGPIPE=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CBodyPipe:
        Q_EMIT insertCommand(QStringLiteral("%BODYPIPE=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CClearPipe:
        Q_EMIT insertCommand(QStringLiteral("%CLEARPIPE=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CCursor:
        Q_EMIT insertCommand(QStringLiteral("%CURSOR"));
        break;
    case TemplatesInsertCommandPushButton::CSignature:
        Q_EMIT insertCommand(QStringLiteral("%SIGNATURE"));
        break;
    case TemplatesInsertCommandPushButton::CInsert:
        Q_EMIT insertCommand(QStringLiteral("%INSERT=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CDnl:
        Q_EMIT insertCommand(QStringLiteral("%-"));
        break;
    case TemplatesInsertCommandPushButton::CRem:
        Q_EMIT insertCommand(QStringLiteral("%REM=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CNop:
        Q_EMIT insertCommand(QStringLiteral("%NOP"));
        break;
    case TemplatesInsertCommandPushButton::CClear:
        Q_EMIT insertCommand(QStringLiteral("%CLEAR"));
        break;
    case TemplatesInsertCommandPushButton::CDebug:
        Q_EMIT insertCommand(QStringLiteral("%DEBUG"));
        break;
    case TemplatesInsertCommandPushButton::CDebugOff:
        Q_EMIT insertCommand(QStringLiteral("%DEBUGOFF"));
        break;
    case TemplatesInsertCommandPushButton::CQuotePlain:
        Q_EMIT insertCommand(QStringLiteral("%FORCEDPLAIN"));
        break;
    case TemplatesInsertCommandPushButton::CQuoteHtml:
        Q_EMIT insertCommand(QStringLiteral("%FORCEDHTML"));
        break;
    case TemplatesInsertCommandPushButton::CDictionaryLanguage:
        Q_EMIT insertCommand(QStringLiteral("%DICTIONARYLANGUAGE=\"\""), -1);
        break;
    case TemplatesInsertCommandPushButton::CLanguage:
        Q_EMIT insertCommand(QStringLiteral("%LANGUAGE=\"\""), -1);
        break;
    default:
        qCDebug(TEMPLATEPARSER_LOG) << "Unknown template command index:" << cmd;
        break;
    }
}
