/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagepartrenderplugin.h"

#include "interfaces/bodyparturlhandler.h"

using namespace MessageViewer;

MessagePartRenderPlugin::~MessagePartRenderPlugin() = default;

const Interface::BodyPartURLHandler *MessagePartRenderPlugin::urlHandler(int idx) const
{
    Q_UNUSED(idx)
    return nullptr;
}
