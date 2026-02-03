/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QtPlugin>

namespace MessageViewer
{
namespace Interface
{
class BodyPartURLHandler;
}
class MessagePartRendererBase;

/*!
 * \class MessageViewer::MessagePartRenderPlugin
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/MessagePartRenderPlugin
 * \brief Plugin interface for MessagePartRendererBase instances.
 */
class MESSAGEVIEWER_EXPORT MessagePartRenderPlugin
{
public:
    /*!
     */
    virtual ~MessagePartRenderPlugin();
    /*!
     */
    virtual MessagePartRendererBase *renderer(int index) = 0;
    /*!
     */
    virtual const Interface::BodyPartURLHandler *urlHandler(int idx) const;
};
}

Q_DECLARE_INTERFACE(MessageViewer::MessagePartRenderPlugin, "org.kde.messageviewer.messagepartrenderer/1.1")
