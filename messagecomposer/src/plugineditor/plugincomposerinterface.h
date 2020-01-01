/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINCOMPOSERINTERFACE_H
#define PLUGINCOMPOSERINTERFACE_H

#include "messagecomposer_export.h"
#include <MessageComposer/ComposerAttachmentInterface>
#include <MessageComposer/ConvertSnippetVariablesUtil>
#include <QObject>
namespace MessageComposer {
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
private:
    MessageComposer::ComposerViewInterface *mComposerViewInterface = nullptr;
};
}
#endif // PLUGINCOMPOSERINTERFACE_H
