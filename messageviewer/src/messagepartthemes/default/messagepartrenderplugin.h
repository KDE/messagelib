/*
   Copyright (c) 2017 Volker Krause <vkrause@kde.org>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
   License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#ifndef MESSAGEVIEWER_MESSAGEPARTRENDERPLUGIN_H
#define MESSAGEVIEWER_MESSAGEPARTRENDERPLUGIN_H

#include "messageviewer_export.h"

#include <QtPlugin>

namespace MessageViewer {
class MessagePartRendererBase;

/**
 * Plugin interface for MessagePartRendererBase instances.
 */
class MESSAGEVIEWER_EXPORT MessagePartRenderPlugin
{
public:
    virtual ~MessagePartRenderPlugin();
    virtual MessagePartRendererBase *renderer(int index) = 0;
};
}

Q_DECLARE_INTERFACE(MessageViewer::MessagePartRenderPlugin, "org.kde.messageviewer.messagepartrenderer/1.0")

#endif // MESSAGEVIEWER_MESSAGEPARTRENDERPLUGIN_H
