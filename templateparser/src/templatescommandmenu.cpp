/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatescommandmenu.h"
using namespace Qt::Literals::StringLiterals;

#include "templateparser_debug.h"
#include <KActionMenu>
#include <KLazyLocalizedString>
#include <KLocalizedString>
#include <QAction>
#include <QMenu>

using namespace TemplateParser;

struct InsertCommand {
    const KLazyLocalizedString name;
    const TemplatesCommandMenu::Command command;

    [[nodiscard]] QString getLocalizedDisplayName() const
    {
        return KLocalizedString(name).toString();
    }
};

static const InsertCommand originalCommands[] = {{kli18n("Quoted Message Text"), TemplatesCommandMenu::CQuote},

                                                 {kli18n("Message Text as Is"), TemplatesCommandMenu::CText},

                                                 {kli18n("Message Id"), TemplatesCommandMenu::COMsgId},

                                                 {kli18n("Date"), TemplatesCommandMenu::CODate},

                                                 {kli18n("Date in Short Format"), TemplatesCommandMenu::CODateShort},

                                                 {kli18n("Date in C Locale"), TemplatesCommandMenu::CODateEn},

                                                 {kli18n("Day of Week"), TemplatesCommandMenu::CODow},

                                                 {kli18n("Time"), TemplatesCommandMenu::COTime},

                                                 {kli18n("Time in Long Format"), TemplatesCommandMenu::COTimeLong},

                                                 {kli18n("Time in C Locale"), TemplatesCommandMenu::COTimeLongEn},

                                                 {kli18n("To Field Address"), TemplatesCommandMenu::COToAddr},

                                                 {kli18n("To Field Name"), TemplatesCommandMenu::COToName},

                                                 {kli18n("To Field First Name"), TemplatesCommandMenu::COToFName},

                                                 {kli18n("To Field Last Name"), TemplatesCommandMenu::COToLName},

                                                 {kli18n("CC Field Address"), TemplatesCommandMenu::COCCAddr},

                                                 {kli18n("CC Field Name"), TemplatesCommandMenu::COCCName},

                                                 {kli18n("CC Field First Name"), TemplatesCommandMenu::COCCFName},

                                                 {kli18n("CC Field Last Name"), TemplatesCommandMenu::COCCLName},

                                                 {kli18n("From Field Address"), TemplatesCommandMenu::COFromAddr},

                                                 {kli18n("From Field Name"), TemplatesCommandMenu::COFromName},

                                                 {kli18n("From Field First Name"), TemplatesCommandMenu::COFromFName},

                                                 {kli18n("From Field Last Name"), TemplatesCommandMenu::COFromLName},

                                                 {kli18n("Addresses of all recipients"), TemplatesCommandMenu::COAddresseesAddr},

                                                 {kli18nc("Template value for subject of the message", "Subject"), TemplatesCommandMenu::COFullSubject},

                                                 {kli18n("Quoted Headers"), TemplatesCommandMenu::CQHeaders},

                                                 {kli18n("Headers as Is"), TemplatesCommandMenu::CHeaders},

                                                 {kli18n("Header Content"), TemplatesCommandMenu::COHeader},

                                                 {kli18n("Reply as Quoted Plain Text"), TemplatesCommandMenu::CQuotePlain},

                                                 {kli18n("Reply as Quoted HTML Text"), TemplatesCommandMenu::CQuoteHtml}};
static const int originalCommandsCount = sizeof(originalCommands) / sizeof(*originalCommands);

static const InsertCommand currentCommands[] = {{kli18n("Date"), TemplatesCommandMenu::CDate},

                                                {kli18n("Date in Short Format"), TemplatesCommandMenu::CDateShort},

                                                {kli18n("Date in C Locale"), TemplatesCommandMenu::CDateEn},

                                                {kli18n("Day of Week"), TemplatesCommandMenu::CDow},

                                                {kli18n("Time"), TemplatesCommandMenu::CTime},

                                                {kli18n("Time in Long Format"), TemplatesCommandMenu::CTimeLong},

                                                {kli18n("Time in C Locale"), TemplatesCommandMenu::CTimeLongEn},
                                                {kli18n("To Field Address"), TemplatesCommandMenu::CToAddr},

                                                {kli18n("To Field Name"), TemplatesCommandMenu::CToName},

                                                {kli18n("To Field First Name"), TemplatesCommandMenu::CToFName},

                                                {kli18n("To Field Last Name"), TemplatesCommandMenu::CToLName},

                                                {kli18n("CC Field Address"), TemplatesCommandMenu::CCCAddr},

                                                {kli18n("CC Field Name"), TemplatesCommandMenu::CCCName},

                                                {kli18n("CC Field First Name"), TemplatesCommandMenu::CCCFName},

                                                {kli18n("CC Field Last Name"), TemplatesCommandMenu::CCCLName},

                                                {kli18n("From Field Address"), TemplatesCommandMenu::CFromAddr},

                                                {kli18n("From field Name"), TemplatesCommandMenu::CFromName},

                                                {kli18n("From Field First Name"), TemplatesCommandMenu::CFromFName},

                                                {kli18n("From Field Last Name"), TemplatesCommandMenu::CFromLName},

                                                {kli18nc("Template subject command.", "Subject"), TemplatesCommandMenu::CFullSubject},

                                                {kli18n("Header Content"), TemplatesCommandMenu::CHeader}};
static const int currentCommandsCount = sizeof(currentCommands) / sizeof(*currentCommands);

static const InsertCommand extCommands[] = {{kli18n("Pipe Original Message Body and Insert Result as Quoted Text"), TemplatesCommandMenu::CQuotePipe},

                                            {kli18n("Pipe Original Message Body and Insert Result as Is"), TemplatesCommandMenu::CTextPipe},

                                            {kli18n("Pipe Original Message with Headers and Insert Result as Is"), TemplatesCommandMenu::CMsgPipe},

                                            {kli18n("Pipe Current Message Body and Insert Result as Is"), TemplatesCommandMenu::CBodyPipe},

                                            {kli18n("Pipe Current Message Body and Replace with Result"), TemplatesCommandMenu::CClearPipe}};

static const int extCommandsCount = sizeof(extCommands) / sizeof(*extCommands);

static const InsertCommand miscCommands[] = {
    {kli18nc("Inserts user signature, also known as footer, into message", "Signature"), TemplatesCommandMenu::CSignature},

    {kli18n("Insert File Content"), TemplatesCommandMenu::CInsert},

    {kli18nc("All characters, up to and including the next newline, "
             "are discarded without performing any macro expansion",
             "Discard to Next Line"),
     TemplatesCommandMenu::CDnl},

    {kli18n("Template Comment"), TemplatesCommandMenu::CRem},

    {kli18n("No Operation"), TemplatesCommandMenu::CNop},

    {kli18n("Clear Generated Message"), TemplatesCommandMenu::CClear},
    {kli18n("Cursor position"), TemplatesCommandMenu::CCursor},

    {kli18n("Blank text"), TemplatesCommandMenu::CBlank},

    {kli18n("Dictionary Language"), TemplatesCommandMenu::CDictionaryLanguage},
    {kli18n("Language"), TemplatesCommandMenu::CLanguage},
    // TODO add support for custom variable. %CUSTOM="???" ?
};
static const int miscCommandsCount = sizeof(miscCommands) / sizeof(*miscCommands);

static const InsertCommand debugCommands[] = {{kli18n("Turn Debug On"), TemplatesCommandMenu::CDebug},

                                              {kli18n("Turn Debug Off"), TemplatesCommandMenu::CDebugOff}};
static const int debugCommandsCount = sizeof(debugCommands) / sizeof(*debugCommands);

void TemplatesCommandMenu::fillMenuFromActionMap(const QMap<QString, TemplatesCommandMenu::Command> &map, KActionMenu *menu)
{
    QMap<QString, TemplatesCommandMenu::Command>::const_iterator it = map.constBegin();
    QMap<QString, TemplatesCommandMenu::Command>::const_iterator end = map.constEnd();

    while (it != end) {
        auto action = new QAction(it.key(), menu); // krazy:exclude=tipsandthis
        const TemplatesCommandMenu::Command cmd = it.value();
        connect(action, &QAction::triggered, this, [this, cmd] {
            slotInsertCommand(cmd);
        });
        menu->addAction(action);
        ++it;
    }
}

TemplatesCommandMenu::TemplatesCommandMenu(QObject *parent)
    : QObject(parent)
{
}

TemplatesCommandMenu::~TemplatesCommandMenu() = default;

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
    QMap<QString, Command> commandMap;
    KActionMenu *menu = nullptr;
    // ******************************************************
    if (mType & ReplyForwardMessage) {
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
    if (mType & CurrentMessage) {
        menu = new KActionMenu(i18n("Current Message"), mMenu);
        mMenu->addAction(menu);

        for (int i = 0; i < currentCommandsCount; ++i) {
            commandMap.insert(currentCommands[i].getLocalizedDisplayName(), currentCommands[i].command);
        }

        fillMenuFromActionMap(commandMap, menu);
        commandMap.clear();
    }

    // ******************************************************
    if (mType & External) {
        menu = new KActionMenu(i18n("Process with External Programs"), mMenu);
        mMenu->addAction(menu);

        for (int i = 0; i < extCommandsCount; ++i) {
            commandMap.insert(extCommands[i].getLocalizedDisplayName(), extCommands[i].command);
        }

        fillMenuFromActionMap(commandMap, menu);
        commandMap.clear();
    }

    // ******************************************************
    if (mType & Misc) {
        menu = new KActionMenu(i18nc("Miscellaneous template commands menu", "Miscellaneous"), mMenu);
        mMenu->addAction(menu);

        for (int i = 0; i < miscCommandsCount; ++i) {
            commandMap.insert(miscCommands[i].getLocalizedDisplayName(), miscCommands[i].command);
        }

        fillMenuFromActionMap(commandMap, menu);
        commandMap.clear();
    }

    // ******************************************************
    if (!qEnvironmentVariableIsEmpty("KDEPIM_DEBUGGING")) {
        if (mType & Debug) {
            menu = new KActionMenu(i18nc("Debug template commands menu", "Debug"), mMenu);
            mMenu->addAction(menu);

            for (int i = 0; i < debugCommandsCount; ++i) {
                commandMap.insert(debugCommands[i].getLocalizedDisplayName(), debugCommands[i].command);
            }

            fillMenuFromActionMap(commandMap, menu);
        }
    }
}

TemplatesCommandMenu::MenuTypes TemplatesCommandMenu::type() const
{
    return mType;
}

void TemplatesCommandMenu::setType(MenuTypes type)
{
    mType = type;
    delete mMenu;
    mWasInitialized = false;
    fillMenu();
    fillSubMenus();
}

void TemplatesCommandMenu::slotInsertCommand(TemplatesCommandMenu::Command cmd)
{
    Q_EMIT insertCommand(cmd);

    switch (cmd) {
    case TemplatesCommandMenu::CBlank:
        Q_EMIT insertCommand(u"%BLANK"_s);
        return;
    case TemplatesCommandMenu::CQuote:
        Q_EMIT insertCommand(u"%QUOTE"_s);
        return;
    case TemplatesCommandMenu::CText:
        Q_EMIT insertCommand(u"%TEXT"_s);
        return;
    case TemplatesCommandMenu::COMsgId:
        Q_EMIT insertCommand(u"%OMSGID"_s);
        return;
    case TemplatesCommandMenu::CODate:
        Q_EMIT insertCommand(u"%ODATE"_s);
        return;
    case TemplatesCommandMenu::CODateShort:
        Q_EMIT insertCommand(u"%ODATESHORT"_s);
        return;
    case TemplatesCommandMenu::CODateEn:
        Q_EMIT insertCommand(u"%ODATEEN"_s);
        return;
    case TemplatesCommandMenu::CODow:
        Q_EMIT insertCommand(u"%ODOW"_s);
        return;
    case TemplatesCommandMenu::COTime:
        Q_EMIT insertCommand(u"%OTIME"_s);
        return;
    case TemplatesCommandMenu::COTimeLong:
        Q_EMIT insertCommand(u"%OTIMELONG"_s);
        return;
    case TemplatesCommandMenu::COTimeLongEn:
        Q_EMIT insertCommand(u"%OTIMELONGEN"_s);
        return;
    case TemplatesCommandMenu::COToAddr:
        Q_EMIT insertCommand(u"%OTOADDR"_s);
        return;
    case TemplatesCommandMenu::COToName:
        Q_EMIT insertCommand(u"%OTONAME"_s);
        return;
    case TemplatesCommandMenu::COToFName:
        Q_EMIT insertCommand(u"%OTOFNAME"_s);
        return;
    case TemplatesCommandMenu::COToLName:
        Q_EMIT insertCommand(u"%OTOLNAME"_s);
        return;
    case TemplatesCommandMenu::COCCAddr:
        Q_EMIT insertCommand(u"%OCCADDR"_s);
        return;
    case TemplatesCommandMenu::COCCName:
        Q_EMIT insertCommand(u"%OCCNAME"_s);
        return;
    case TemplatesCommandMenu::COCCFName:
        Q_EMIT insertCommand(u"%OCCFNAME"_s);
        return;
    case TemplatesCommandMenu::COCCLName:
        Q_EMIT insertCommand(u"%OCCLNAME"_s);
        return;
    case TemplatesCommandMenu::COFromAddr:
        Q_EMIT insertCommand(u"%OFROMADDR"_s);
        return;
    case TemplatesCommandMenu::COFromName:
        Q_EMIT insertCommand(u"%OFROMNAME"_s);
        return;
    case TemplatesCommandMenu::COFromFName:
        Q_EMIT insertCommand(u"%OFROMFNAME"_s);
        return;
    case TemplatesCommandMenu::COFromLName:
        Q_EMIT insertCommand(u"%OFROMLNAME"_s);
        return;
    case TemplatesCommandMenu::COFullSubject:
        Q_EMIT insertCommand(u"%OFULLSUBJECT"_s);
        return;
    case TemplatesCommandMenu::CQHeaders:
        Q_EMIT insertCommand(u"%QHEADERS"_s);
        return;
    case TemplatesCommandMenu::CHeaders:
        Q_EMIT insertCommand(u"%HEADERS"_s);
        return;
    case TemplatesCommandMenu::COHeader:
        Q_EMIT insertCommand(u"%OHEADER=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CMsgId:
        Q_EMIT insertCommand(u"%MSGID"_s);
        return;
    case TemplatesCommandMenu::CDate:
        Q_EMIT insertCommand(u"%DATE"_s);
        return;
    case TemplatesCommandMenu::CDateShort:
        Q_EMIT insertCommand(u"%DATESHORT"_s);
        return;
    case TemplatesCommandMenu::CDateEn:
        Q_EMIT insertCommand(u"%DATEEN"_s);
        return;
    case TemplatesCommandMenu::CDow:
        Q_EMIT insertCommand(u"%DOW"_s);
        return;
    case TemplatesCommandMenu::CTime:
        Q_EMIT insertCommand(u"%TIME"_s);
        return;
    case TemplatesCommandMenu::CTimeLong:
        Q_EMIT insertCommand(u"%TIMELONG"_s);
        return;
    case TemplatesCommandMenu::CTimeLongEn:
        Q_EMIT insertCommand(u"%TIMELONGEN"_s);
        return;
    case TemplatesCommandMenu::COAddresseesAddr:
        Q_EMIT insertCommand(u"%OADDRESSEESADDR"_s);
        return;
    case TemplatesCommandMenu::CToAddr:
        Q_EMIT insertCommand(u"%TOADDR"_s);
        return;
    case TemplatesCommandMenu::CToName:
        Q_EMIT insertCommand(u"%TONAME"_s);
        return;
    case TemplatesCommandMenu::CToFName:
        Q_EMIT insertCommand(u"%TOFNAME"_s);
        return;
    case TemplatesCommandMenu::CToLName:
        Q_EMIT insertCommand(u"%TOLNAME"_s);
        return;
    case TemplatesCommandMenu::CCCAddr:
        Q_EMIT insertCommand(u"%CCADDR"_s);
        return;
    case TemplatesCommandMenu::CCCName:
        Q_EMIT insertCommand(u"%CCNAME"_s);
        return;
    case TemplatesCommandMenu::CCCFName:
        Q_EMIT insertCommand(u"%CCFNAME"_s);
        return;
    case TemplatesCommandMenu::CCCLName:
        Q_EMIT insertCommand(u"%CCLNAME"_s);
        return;
    case TemplatesCommandMenu::CFromAddr:
        Q_EMIT insertCommand(u"%FROMADDR"_s);
        return;
    case TemplatesCommandMenu::CFromName:
        Q_EMIT insertCommand(u"%FROMNAME"_s);
        return;
    case TemplatesCommandMenu::CFromFName:
        Q_EMIT insertCommand(u"%FROMFNAME"_s);
        return;
    case TemplatesCommandMenu::CFromLName:
        Q_EMIT insertCommand(u"%FROMLNAME"_s);
        return;
    case TemplatesCommandMenu::CFullSubject:
        Q_EMIT insertCommand(u"%FULLSUBJECT"_s);
        return;
    case TemplatesCommandMenu::CHeader:
        Q_EMIT insertCommand(u"%HEADER=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CSystem:
        Q_EMIT insertCommand(u"%SYSTEM=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CQuotePipe:
        Q_EMIT insertCommand(u"%QUOTEPIPE=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CTextPipe:
        Q_EMIT insertCommand(u"%TEXTPIPE=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CMsgPipe:
        Q_EMIT insertCommand(u"%MSGPIPE=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CBodyPipe:
        Q_EMIT insertCommand(u"%BODYPIPE=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CClearPipe:
        Q_EMIT insertCommand(u"%CLEARPIPE=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CCursor:
        Q_EMIT insertCommand(u"%CURSOR"_s);
        return;
    case TemplatesCommandMenu::CSignature:
        Q_EMIT insertCommand(u"%SIGNATURE"_s);
        return;
    case TemplatesCommandMenu::CInsert:
        Q_EMIT insertCommand(u"%INSERT=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CDnl:
        Q_EMIT insertCommand(u"%-"_s);
        return;
    case TemplatesCommandMenu::CRem:
        Q_EMIT insertCommand(u"%REM=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CNop:
        Q_EMIT insertCommand(u"%NOP"_s);
        return;
    case TemplatesCommandMenu::CClear:
        Q_EMIT insertCommand(u"%CLEAR"_s);
        return;
    case TemplatesCommandMenu::CDebug:
        Q_EMIT insertCommand(u"%DEBUG"_s);
        return;
    case TemplatesCommandMenu::CDebugOff:
        Q_EMIT insertCommand(u"%DEBUGOFF"_s);
        return;
    case TemplatesCommandMenu::CQuotePlain:
        Q_EMIT insertCommand(u"%FORCEDPLAIN"_s);
        return;
    case TemplatesCommandMenu::CQuoteHtml:
        Q_EMIT insertCommand(u"%FORCEDHTML"_s);
        return;
    case TemplatesCommandMenu::CDictionaryLanguage:
        Q_EMIT insertCommand(u"%DICTIONARYLANGUAGE=\"\""_s, -1);
        return;
    case TemplatesCommandMenu::CLanguage:
        Q_EMIT insertCommand(u"%LANGUAGE=\"\""_s, -1);
        return;
    }
    qCDebug(TEMPLATEPARSER_LOG) << "Unknown template command index:" << cmd;
}

#include "moc_templatescommandmenu.cpp"
