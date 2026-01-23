/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <MessageComposer/PluginEditorBase>

namespace MessageComposer
{
class PluginEditorCheckBeforeSendInterface;
/*!
 \class MessageComposer::PluginEditorCheckBeforeSend
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorCheckBeforeSend
 \author Laurent Montel <montel@kde.org>

 The PluginEditorCheckBeforeSend class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSend : public PluginEditorBase
{
    Q_OBJECT
public:
    /*!
     */
    explicit PluginEditorCheckBeforeSend(QObject *parent = nullptr);
    /*!
     */
    ~PluginEditorCheckBeforeSend() override;

    /*!
     */
    virtual PluginEditorCheckBeforeSendInterface *createInterface(QObject *parent) = 0;
};
}
