/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CONVERTSNIPPETVARIABLEMENU_H
#define CONVERTSNIPPETVARIABLEMENU_H

#include <QObject>
#include "messagecomposer_export.h"
#include <MessageComposer/ConvertSnippetVariablesUtil>
class QMenu;
namespace MessageComposer {
/**
 * @brief The ConvertSnippetVariableMenu class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ConvertSnippetVariableMenu : public QObject
{
    Q_OBJECT
public:
    explicit ConvertSnippetVariableMenu(bool onlyMenuForCustomizeAttachmentFileName, QWidget *parentWidget, QObject *parent = nullptr);
    ~ConvertSnippetVariableMenu();
    QMenu *menu() const;

Q_SIGNALS:
    void insertVariable(MessageComposer::ConvertSnippetVariablesUtil::VariableType type);
private:
    void initializeMenu(bool onlyMenuForCustomizeAttachmentFileName);
    QWidget *mParentWidget = nullptr;
    QMenu *mMenu = nullptr;
};
}
#endif // CONVERTSNIPPETVARIABLEMENU_H
