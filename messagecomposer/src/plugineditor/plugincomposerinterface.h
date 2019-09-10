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

#ifndef PLUGINCOMPOSERINTERFACE_H
#define PLUGINCOMPOSERINTERFACE_H

#include "messagecomposer_export.h"
#include "pluginattachmentinterface.h"
#include <QObject>
namespace MessageComposer {
class ComposerViewBase;
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
    Q_REQUIRED_RESULT PluginAttachmentInterface attachments();
private:
    //Make it as private as we don't want that plugin has too many action on composer
    Q_REQUIRED_RESULT ComposerViewBase *composerViewBase() const;
    ComposerViewBase *mComposerViewBase = nullptr;
};
}
#endif // PLUGINCOMPOSERINTERFACE_H
