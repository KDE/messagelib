/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
