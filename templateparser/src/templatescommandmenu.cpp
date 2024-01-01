/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatescommandmenu.h"

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

#include "moc_templatescommandmenu.cpp"
