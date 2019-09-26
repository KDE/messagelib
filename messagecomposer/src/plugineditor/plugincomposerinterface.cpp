/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "plugincomposerinterface.h"
#include "composer/composerviewbase.h"
#include "composer/composerviewinterface.h"
#include "composer/composerattachmentinterface.h"
#include <MessageComposer/AttachmentModel>
#include <MessageComposer/ConvertSnippetVariablesJob>
#include <KFormat>
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

QString PluginComposerInterface::subject() const
{
    return mComposerViewInterface->subject();
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

QString PluginComposerInterface::convertVariable(MessageComposer::ConvertSnippetVariablesUtil::VariableType type) const
{
    return MessageComposer::ConvertSnippetVariablesJob::convertVariables(mComposerViewInterface, MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(type));
}

QString PluginComposerInterface::convertText(const QString &str) const
{
    return MessageComposer::ConvertSnippetVariablesJob::convertVariables(mComposerViewInterface, str);
}
