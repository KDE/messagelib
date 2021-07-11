/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugincomposerinterface.h"
#include "composer/composerattachmentinterface.h"
#include "composer/composerviewbase.h"
#include "composer/composerviewinterface.h"
#include <MessageComposer/AttachmentModel>
#include <MessageComposer/ConvertSnippetVariablesJob>
using namespace MessageComposer;

PluginComposerInterface::PluginComposerInterface()
{
}

PluginComposerInterface::~PluginComposerInterface()
{
    delete mComposerViewInterface;
}

void PluginComposerInterface::setComposerViewBase(ComposerViewBase *composerViewBase)
{
    delete mComposerViewInterface;
    mComposerViewInterface = new MessageComposer::ComposerViewInterface(composerViewBase);
}

void PluginComposerInterface::setReplyTo(const QString &str)
{
    mComposerViewInterface->setReplyTo(str);
}

void PluginComposerInterface::setSubject(const QString &str)
{
    mComposerViewInterface->setSubject(str);
}

void PluginComposerInterface::setTo(const QStringList &lst)
{
    mComposerViewInterface->setTo(lst);
}

void PluginComposerInterface::setCc(const QStringList &lst)
{
    mComposerViewInterface->setCc(lst);
}

void PluginComposerInterface::setFrom(const QString &str)
{
    mComposerViewInterface->setFrom(str);
}

QString PluginComposerInterface::subject() const
{
    return mComposerViewInterface->subject();
}

QString PluginComposerInterface::replyTo() const
{
    return mComposerViewInterface->replyTo();
}

QString PluginComposerInterface::to() const
{
    return mComposerViewInterface->to();
}

QString PluginComposerInterface::cc() const
{
    return mComposerViewInterface->cc();
}

QString PluginComposerInterface::from() const
{
    return mComposerViewInterface->from();
}

MessageComposer::ComposerAttachmentInterface PluginComposerInterface::attachments()
{
    return mComposerViewInterface->attachments();
}

QString PluginComposerInterface::shortDate() const
{
    return mComposerViewInterface->shortDate();
}

QString PluginComposerInterface::longDate() const
{
    return mComposerViewInterface->longDate();
}

QString PluginComposerInterface::shortTime() const
{
    return mComposerViewInterface->shortTime();
}

QString PluginComposerInterface::longTime() const
{
    return mComposerViewInterface->longTime();
}

QString PluginComposerInterface::insertDayOfWeek() const
{
    return mComposerViewInterface->insertDayOfWeek();
}

QString PluginComposerInterface::variableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type) const
{
    return MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(type);
}

QString PluginComposerInterface::convertVariable(MessageComposer::ConvertSnippetVariablesUtil::VariableType type) const
{
    return MessageComposer::ConvertSnippetVariablesJob::convertVariables(mComposerViewInterface,
                                                                         MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(type));
}

QString PluginComposerInterface::convertText(const QString &str) const
{
    return MessageComposer::ConvertSnippetVariablesJob::convertVariables(mComposerViewInterface, str);
}
