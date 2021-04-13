/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <MessageComposer/ConvertSnippetVariablesUtil>
#include <QObject>
class QMenu;
namespace MessageComposer
{
/**
 * @brief The ConvertSnippetVariableMenu class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ConvertSnippetVariableMenu : public QObject
{
    Q_OBJECT
public:
    explicit ConvertSnippetVariableMenu(bool onlyMenuForCustomizeAttachmentFileName, QWidget *parentWidget, QObject *parent = nullptr);
    ~ConvertSnippetVariableMenu() override;
    Q_REQUIRED_RESULT QMenu *menu() const;

Q_SIGNALS:
    void insertVariable(MessageComposer::ConvertSnippetVariablesUtil::VariableType type);

private:
    void initializeMenu(bool onlyMenuForCustomizeAttachmentFileName);
    QWidget *const mParentWidget;
    QMenu *mMenu = nullptr;
};
}
