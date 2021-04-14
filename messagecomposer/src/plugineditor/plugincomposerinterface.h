/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <MessageComposer/ComposerAttachmentInterface>
#include <MessageComposer/ConvertSnippetVariablesUtil>
namespace MessageComposer
{
class ComposerViewBase;
class ComposerViewInterface;
/**
 * @brief The PluginComposerInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginComposerInterface
{
public:
    PluginComposerInterface();
    ~PluginComposerInterface();
    void setComposerViewBase(ComposerViewBase *composerViewBase);

    Q_REQUIRED_RESULT QString subject() const;
    Q_REQUIRED_RESULT QString to() const;
    Q_REQUIRED_RESULT QString cc() const;
    Q_REQUIRED_RESULT QString from() const;
    Q_REQUIRED_RESULT ComposerAttachmentInterface attachments();
    Q_REQUIRED_RESULT QString shortDate() const;
    Q_REQUIRED_RESULT QString longDate() const;
    Q_REQUIRED_RESULT QString shortTime() const;
    Q_REQUIRED_RESULT QString longTime() const;
    Q_REQUIRED_RESULT QString insertDayOfWeek() const;

    Q_REQUIRED_RESULT QString convertVariable(MessageComposer::ConvertSnippetVariablesUtil::VariableType type) const;
    Q_REQUIRED_RESULT QString convertText(const QString &str) const;

    Q_REQUIRED_RESULT QString variableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type) const;

private:
    MessageComposer::ComposerViewInterface *mComposerViewInterface = nullptr;
};
}
