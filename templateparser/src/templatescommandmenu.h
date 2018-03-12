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

#ifndef TEMPLATESCOMMANDMENU_H
#define TEMPLATESCOMMANDMENU_H

#include <QMap>
#include <QObject>
#include "templateparser_private_export.h"
class KActionMenu;
class QMenu;
namespace TemplateParser {
class TEMPLATEPARSER_TESTS_EXPORT TemplatesCommandMenu : public QObject
{
    Q_OBJECT
public:
    explicit TemplatesCommandMenu(QObject *parent = nullptr);
    ~TemplatesCommandMenu();

    enum MenuType {
        ReplyForwardMessage,
        NewMessage,
        All
    };

    //TODO: apidox for all these enums
    enum Command {
        CDnl = 1,
        CRem,
        CInsert,
        CSystem,
        CQuotePipe,
        CQuote,
        CQHeaders,
        CHeaders,
        CTextPipe,
        CMsgPipe,
        CBodyPipe,
        CClearPipe,
        CText,
        CToAddr,
        CToName,
        CFromAddr,
        CFromName,
        CFullSubject,
        CMsgId,
        COHeader,
        CHeader,
        COToAddr,
        COToName,
        COFromAddr,
        COFromName,
        COFullSubject,
        COMsgId,
        CDateEn,
        CDateShort,
        CDate,
        CDow,
        CTimeLongEn,
        CTimeLong,
        CTime,
        CODateEn,
        CODateShort,
        CODate,
        CODow,
        COTimeLongEn,
        COTimeLong,
        COTime,
        CBlank,
        CNop,
        CClear,
        CDebug,
        CDebugOff,
        CToFName,
        CToLName,
        CFromFName,
        CFromLName,
        COToFName,
        COToLName,
        COFromFName,
        COFromLName,
        CCursor,
        CCCAddr,
        CCCName,
        CCCFName,
        CCCLName,
        COCCAddr,
        COCCName,
        COCCFName,
        COCCLName,
        COAddresseesAddr,
        CSignature,
        CQuotePlain,
        CQuoteHtml,
        CDictionaryLanguage,
        CLanguage
    };

    QMenu *menu() const;
    void fillMenu();
    void fillSubMenus();

    MenuType type() const;
    void setType(const MenuType &type);

Q_SIGNALS:
    void insertCommand(TemplatesCommandMenu::Command cmd);
    void insertCommand(const QString &cmd, int adjustCursor = 0);

public Q_SLOTS:
    void slotInsertCommand(TemplatesCommandMenu::Command cmd);

protected:
    KActionMenu *mMenu = nullptr;

private:
    void fillMenuFromActionMap(const QMap<QString, TemplatesCommandMenu::Command> &map, KActionMenu *menu);
    MenuType mType = All;
    bool mWasInitialized = false;
};
}

#endif // TEMPLATESCOMMANDMENU_H
