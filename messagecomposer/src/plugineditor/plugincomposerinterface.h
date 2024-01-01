/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] QString replyTo() const;
    [[nodiscard]] QString subject() const;
    [[nodiscard]] QString to() const;
    [[nodiscard]] QString cc() const;
    [[nodiscard]] QString from() const;
    [[nodiscard]] ComposerAttachmentInterface attachments();
    [[nodiscard]] QString shortDate() const;
    [[nodiscard]] QString longDate() const;
    [[nodiscard]] QString shortTime() const;
    [[nodiscard]] QString longTime() const;
    [[nodiscard]] QString insertDayOfWeek() const;

    [[nodiscard]] QString convertVariable(MessageComposer::ConvertSnippetVariablesUtil::VariableType type) const;
    [[nodiscard]] QString convertText(const QString &str) const;

    [[nodiscard]] QString variableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type) const;

private:
    MessageComposer::ComposerViewInterface *mComposerViewInterface = nullptr;
};
}
