/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class TemplateParser::TemplatesCommandMenu
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/TemplatesCommandMenu
 * \brief The TemplatesCommandMenu class
 * \author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesCommandMenu : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent object
     */
    explicit TemplatesCommandMenu(QObject *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~TemplatesCommandMenu() override;

    enum MenuType : uint8_t {
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
    enum Command : uint8_t {
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

    /*!
     * \brief Returns the menu
     * \return QMenu pointer
     */
    [[nodiscard]] QMenu *menu() const;
    /*!
     * \brief Fills the menu with commands
     */
    void fillMenu();
    /*!
     * \brief Fills the submenus
     */
    void fillSubMenus();

    /*!
     * \brief Returns the menu types
     * \return menu types flags
     */
    [[nodiscard]] MenuTypes type() const;
    /*!
     * \brief Sets the menu types
     * \param type menu types flags
     */
    void setType(MenuTypes type);

Q_SIGNALS:
    /*!
     * \brief Emitted when a command is inserted
     * \param cmd command enumeration
     */
    void insertCommand(TemplateParser::TemplatesCommandMenu::Command cmd);
    /*!
     * \brief Emitted when a command string is inserted
     * \param cmd command string
     * \param adjustCursor cursor adjustment value
     */
    void insertCommand(const QString &cmd, int adjustCursor = 0);

public Q_SLOTS:
    /*!
     * \brief Slot to insert a command
     * \param cmd command enumeration
     */
    void slotInsertCommand(TemplateParser::TemplatesCommandMenu::Command cmd);

protected:
    KActionMenu *mMenu = nullptr;

private:
    TEMPLATEPARSER_NO_EXPORT void fillMenuFromActionMap(const QMap<QString, TemplatesCommandMenu::Command> &map, KActionMenu *menu);
    MenuTypes mType = Default;
    bool mWasInitialized = false;
};
}
