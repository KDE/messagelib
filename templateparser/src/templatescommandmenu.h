/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"
#include <QMap>
#include <QObject>
class KActionMenu;
class QMenu;
namespace TemplateParser
{
/**
 * @brief The TemplatesCommandMenu class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesCommandMenu : public QObject
{
    Q_OBJECT
public:
    explicit TemplatesCommandMenu(QObject *parent = nullptr);
    ~TemplatesCommandMenu() override;

    enum MenuType {
        ReplyForwardMessage = 1,
        CurrentMessage = 2,
        External = 4,
        Misc = 8,
        Debug = 16,
        Default = ReplyForwardMessage | CurrentMessage | External | Misc | Debug,
    };
    Q_FLAG(MenuType)
    Q_DECLARE_FLAGS(MenuTypes, MenuType)
    // TODO: apidox for all these enums
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
        CLanguage,
    };

    [[nodiscard]] QMenu *menu() const;
    void fillMenu();
    void fillSubMenus();

    [[nodiscard]] MenuTypes type() const;
    void setType(MenuTypes type);

Q_SIGNALS:
    void insertCommand(TemplatesCommandMenu::Command cmd);
    void insertCommand(const QString &cmd, int adjustCursor = 0);

public Q_SLOTS:
    void slotInsertCommand(TemplatesCommandMenu::Command cmd);

protected:
    KActionMenu *mMenu = nullptr;

private:
    TEMPLATEPARSER_NO_EXPORT void fillMenuFromActionMap(const QMap<QString, TemplatesCommandMenu::Command> &map, KActionMenu *menu);
    MenuTypes mType = Default;
    bool mWasInitialized = false;
};
}
